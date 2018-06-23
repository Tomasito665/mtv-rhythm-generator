#include "MTVRhythmSpace.h"
#include <exception>
#include <algorithm>
#include <cmath>

MTVRhythmSpace::MTVRhythmSpace(const TimeSignature& ts, UnitRef stepUnit) : 
  mReady(false),
  mTs(ts),
  mStepUnit(stepUnit),
  mNSteps(ts.getExactMeasureDuration(stepUnit)),
  mNPoints((int)std::pow(2.f, mNSteps)),
  mNormDist(std::normal_distribution<double>(0.0, MTV_RHYTHM_SPACE_RAND_SIGMA))
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
  
  // compute tension vectors for all possible rhythm patterns (with this 
  // space's time signature and step unit)
  for (PatternId patternId = 0; patternId < mNPoints; ++patternId) {
    assert(patternId == mPoints.size());
    const RhythmPattern r(mTs, mStepUnit, patternId);
    Tension* mtv = new Tension[mNSteps];
    computeMTV(r, prf, prfRange, mtv);
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
  return mDistanceCache.front().second;
}

PatternId MTVRhythmSpace::getRandomPatternCloseTo(const Tension * const mtv)
{
  if (!equalsDistanceCacheTargetPoint(mtv))
    updateDistanceCache(mtv);
  
  const double normalAbsValue = std::min(std::abs(mNormDist(mRandom)), 1.0);
  const size_t index = (size_t)((double)mNPoints * normalAbsValue);
  return mDistanceCache[index].second;
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
    const Tension * const mtv = mPoints[(size_t)patternId];
    double totalSquaredDiff = 0.0;

    for (int pos = 0; pos < mNSteps; ++pos) {
      const Tension delta = mtv[pos] - targetMtv[pos];
      totalSquaredDiff += delta * delta;
    }

    const float distance = (float)std::sqrt(totalSquaredDiff);
    mDistanceCache.push_back({distance, patternId});
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
