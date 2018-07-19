#include "TimeSignature.h"
#include <functional>
#include <stdexcept>
#include <assert.h>
#include <algorithm>

using namespace std::placeholders;

TimeSignature::TimeSignature() : TimeSignature(4, 4)
{
}

TimeSignature::TimeSignature(int numerator, int denominator) :
  mNumerator(numerator),
  mDenominator(denominator),
  mBeatUnit(Unit::get(denominator))
{
  if (!mBeatUnit) {
    char msg[64];
    sprintf_s(msg, "illegal time signature denominator %d", denominator);
    throw std::runtime_error(msg);
  }
}

double TimeSignature::getMeasureDuration(UnitRef unit) const
{
  return mBeatUnit->convert(mNumerator, unit);
}

int TimeSignature::getExactMeasureDuration(UnitRef unit) const
{
  return mBeatUnit->convertExact(mNumerator, unit);
}

MeterSubdivisionList TimeSignature::getHierarchicalMeterSubdivisions(UnitRef stepUnit) const
{
  checkStepUnit(stepUnit);
  MeterSubdivisionList subdivisions;
  int modDeterminer, binaryMod, tripleMod, sub;
  int n = getExactMeasureDuration(stepUnit);

  while (n > 1) {
    modDeterminer = stepUnit->convertExact(n, mBeatUnit);
    if (modDeterminer <= 1) modDeterminer = n;

    if ((binaryMod = modDeterminer % 2) == 0)
      sub = 2;
    else if ((tripleMod = modDeterminer % 3) == 0)
      sub = 3;
    else {
      char msg[110];
      sprintf_s(msg, "can't create hierarchical subdivisions "
        "for context-sensitive meter %d/%d", mNumerator, mDenominator);
      throw std::runtime_error(msg);
    }

    subdivisions.push_back(sub);
    n /= sub;
  }

  subdivisions.shrink_to_fit();
  return subdivisions;
}

MetricalSalienceProfile TimeSignature::getMetricalSalienceProfile(UnitRef stepUnit, int rootWeight) const
{
  return createMeterMap<int8_t>(stepUnit, [rootWeight](const MeterTreeNodeInfo& node) -> int8_t {
    return rootWeight - node.depth;
  });
}

NaturalDurationsList TimeSignature::getNaturalDurationsMap(UnitRef stepUnit, bool trimToBeat, NaturalDurationsList* naturalDurationsPoolOut) const
{
  const int nStepsPerBeat = mBeatUnit->convertExact(1, stepUnit);
  
  auto naturalDurationsMap = createMeterMap<int8_t>(stepUnit, [nStepsPerBeat, trimToBeat](const MeterTreeNodeInfo& node) -> int8_t {
    const int naturalDuration = node.treeWidth / node.nNodesOnSameLevel;
    return trimToBeat ? std::min(naturalDuration, nStepsPerBeat) : naturalDuration;
  }, naturalDurationsPoolOut);

  // when trimming to beat, there may be duplicate durations in the duration pool, which need to be removed
  if (naturalDurationsPoolOut && trimToBeat) {
    auto last = std::unique(naturalDurationsPoolOut->begin(), naturalDurationsPoolOut->end());
    naturalDurationsPoolOut->erase(last, naturalDurationsPoolOut->end());
  }

  return naturalDurationsMap;
}

void TimeSignature::checkStepUnit(UnitRef stepUnit) const
{
  if ((*stepUnit) > (*mBeatUnit)) {
    char msg[114];
    sprintf_s(msg, "%d/%d meter not representable in %d/%d", mNumerator, mDenominator,
      stepUnit->getNumerator(), stepUnit->getDenominator());
    throw std::runtime_error(msg);
  }
}
