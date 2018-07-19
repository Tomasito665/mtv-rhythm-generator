#include "MTVRhythmSpace.h"
#include <exception>
#include <algorithm>
#include <cmath>
#include <limits>


MTVRhythmSpace::MTVRhythmSpace(const TimeSignature& ts, UnitRef stepUnit) : 
  mReady(false),
  mTs(ts),
  mStepUnit(stepUnit),
  mNSteps(ts.getExactMeasureDuration(stepUnit)),
  mNPoints((int)std::pow(2.f, mNSteps))
{
  ts.checkStepUnit(stepUnit);
  mPoints.reserve(mNPoints);
  mDistanceCache.reserve(mNPoints);
  mDistanceCacheTargetPoint = new Tension[mNSteps];  
}

MTVRhythmSpace::~MTVRhythmSpace()
{
  auto pointIt = mPoints.begin();

  while (pointIt != mPoints.end()) {
    const Tension* const mtv = *pointIt;
    delete[] mtv;
    pointIt = mPoints.erase(pointIt);
  }
  
  delete[] mDistanceCacheTargetPoint;
}

void MTVRhythmSpace::fill(std::function<void(double)> progressFuncCallback)
{
  if (mReady) return;
  assert(mPoints.empty());

  // get metrical salience profile and find the minimum salience
  std::pair<MetricalSalience, MetricalSalience> prfRange;
  prfRange.second = 0;  // <- use use this as root, making this the max salience
  const MetricalSalienceProfile prf = mTs.getMetricalSalienceProfile(mStepUnit, prfRange.second);
  prfRange.first = *std::min_element(prf.begin(), prf.begin() + mNSteps);

  // create one rhythm pattern object and reuse it to compute MTVs for all combinations
  RhythmPattern rp(mTs, mStepUnit);
  
  // compute tension vectors for all possible rhythm patterns (with this 
  // space's time signature and step unit)
  for (PatternId patternId = 0; patternId < mNPoints; ++patternId) {
    assert(patternId == mPoints.size());
    rp.setPatternId(patternId);
    Tension* mtv = new Tension[mNSteps];
    computeMTV(rp, prf, prfRange, mtv);
    mPoints.push_back(mtv);
    if (progressFuncCallback)
      progressFuncCallback(((double)patternId + 1.0) / mNPoints);
  }

  mReady = true;
}

const Tension * const MTVRhythmSpace::getMTV(const PatternId patternId) const
{
  checkIfReady();

  try {
    return const_cast<const Tension * const>(mPoints[(size_t)patternId]);
  }
  catch (const std::out_of_range&) {
    return nullptr;
  }
}

int MTVRhythmSpace::getPatternCount() const
{
  return mNPoints;
}

int MTVRhythmSpace::getDimensions() const
{
  return mNSteps;
}

PatternId MTVRhythmSpace::getClosestPattern(const Tension * const mtv)
{
  if (!equalsDistanceCacheTargetPoint(mtv))
    updateDistanceCache(mtv);
  return mDistanceCache.front().patternId;
}

PatternId MTVRhythmSpace::getRandomPatternCloseTo(const Tension * const mtv, float distanceSD)
{
  if (!equalsDistanceCacheTargetPoint(mtv))
    updateDistanceCache(mtv);
  
  std::normal_distribution<float> normDist(0.0, distanceSD);
  const float randomDistance = std::min(std::abs(normDist(mRandom)), 1.0f);

  // Find the first pattern exceeding the target distance
  std::vector<DistanceCacheEntry>::iterator lowerBound = std::lower_bound(
    mDistanceCache.begin(), mDistanceCache.end(), randomDistance);
  
  // The first pattern exceedig the target is not necesarily the closest one to the target (it may 
  // also be the previous cluster of patterns). Check if this is the case and if so, re-find the 
  // lower bound of the previous cluster's distance to the mtv.
  if (lowerBound != mDistanceCache.begin()) {
    const float prevDistanceToTarget = (lowerBound - 1)->distanceToTarget;
    const float delta = std::abs(lowerBound->distanceToTarget - randomDistance);
    const float prevDelta = std::abs(prevDistanceToTarget - randomDistance);

    if (prevDelta < delta)
      lowerBound = std::lower_bound(mDistanceCache.begin(), lowerBound, prevDistanceToTarget);
  }
  
  // Find the upper bound of the cluster of patterns with equal distances to the target mtv
  std::vector<DistanceCacheEntry>::iterator upperBound = std::upper_bound(
    lowerBound, mDistanceCache.end(), lowerBound->distanceToTarget);

  const int lowerBoundPos = lowerBound - mDistanceCache.begin();
  const int upperBoundPos = upperBound - mDistanceCache.begin() - 1;  // -1 for [min, max] ( not [min, max) )

  std::uniform_int_distribution<int> uniDist(lowerBoundPos, upperBoundPos);
  return mDistanceCache[uniDist(mRandom)].patternId;
}

float MTVRhythmSpace::getDistance(const Tension * const mtvA, const Tension * const mtvB) const
{
  double totalSquaredDiff = 0.0;

  for (int pos = 0; pos < mNSteps; ++pos) {
    const Tension delta = mtvA[pos] - mtvB[pos];
    totalSquaredDiff += delta * delta;
  }

  float distance = (float)std::sqrt(totalSquaredDiff);
  return distance / (float)std::sqrt(mNSteps); // normalize to [0, 1]
}

void MTVRhythmSpace::checkIfReady() const
{
  if (!mReady) {
    throw std::runtime_error("MTVRhythmSpace not ready yet");
  }
}

void MTVRhythmSpace::updateDistanceCache(const Tension * const targetMtv)
{
  checkIfReady();
  mDistanceCache.clear();

  for (PatternId patternId = 0; patternId < mNPoints; ++patternId) {
    const float distance = getDistance(targetMtv, mPoints[(size_t)patternId]);
    mDistanceCache.push_back({ distance, patternId });
  }

  // sorts the distances using the default std::pair comparator, which first compares 
  // the first element (distance) and then the second
  std::sort(mDistanceCache.begin(), mDistanceCache.end());

  // update cache target point
  for (int pos = 0; pos < mNSteps; ++pos)
    mDistanceCacheTargetPoint[pos] = targetMtv[pos];
}

bool MTVRhythmSpace::equalsDistanceCacheTargetPoint(const Tension * const mtv) const
{
  if (!mDistanceCacheTargetPoint)
    return false;

  for (int i = 0; i < mNSteps; ++i) {
    if (mtv[i] != mDistanceCacheTargetPoint[i])
      return false;
  }

  return true;
}


void computeMTV(
  const RhythmPattern & rhythm, 
  const MetricalSalienceProfile & prf, 
  MetricalSalienceRange salienceRange,
  Tension * mtvOut
)
{
  MetricalSalience minSalience = salienceRange.first;
  MetricalSalience maxSalience = salienceRange.second;
  MetricalSalience salienceDelta = maxSalience - minSalience;

  const int nSteps = rhythm.getNSteps();
  const MusicalEventList& events = rhythm.asMusicalEvents();
  const MusicalEvent* currEvent = &events.front() - 1;  // one before first element
  const MusicalEvent* prevEvent = nullptr;

  Tension currEventTension = -1;
  int currEventEndPos = -1;

  for (int pos = 0; pos < nSteps; ++pos) {
    if (pos > currEventEndPos) {
      prevEvent = prevEvent ? currEvent : &events.back();
      currEventEndPos = (++currEvent)->getTrailingPosition();

      const int salience = currEvent->type == MusicalEventType::TIED_NOTE
        ? prf[prevEvent->position]
        : prf[currEvent->position];

      const double relSalience = (double)(salience - minSalience) / salienceDelta;
      currEventTension = (Tension)(1.0 - relSalience);
    }

    mtvOut[pos] = currEventTension;
  }
}
