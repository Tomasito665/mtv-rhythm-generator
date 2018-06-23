#include "RhythmPattern.h"
#include <stdexcept>
#include <assert.h>


RhythmPattern::RhythmPattern(const TimeSignature& ts, UnitRef stepUnit, PatternId pattern) : 
  mTs(ts),
  mStepUnit(stepUnit),
  mNSteps(ts.getExactMeasureDuration(stepUnit)),
  mPattern(std::bitset<MAX_N_STEPS>(pattern))
{
  ts.checkStepUnit(stepUnit);
  checkNSteps();
}

RhythmPattern::~RhythmPattern() { }

bool RhythmPattern::operator[](size_t index) const
{
  checkStepIndex(index);
  return mPattern[index];
}

std::bitset<MAX_N_STEPS>::reference RhythmPattern::operator[](size_t index)
{
  checkStepIndex(index);
  return mPattern[index];
}

PatternId RhythmPattern::getPatternId() const
{
  return (PatternId)mPattern.to_ullong();
}

bool RhythmPattern::operator==(const RhythmPattern& other) const
{
  return mTs == other.mTs
    && mStepUnit == other.mStepUnit
    && mPattern == other.mPattern;
}

bool RhythmPattern::operator!=(const RhythmPattern& other) const
{
  return !(*this == other);
}

void RhythmPattern::toggle(size_t index)
{
  checkStepIndex(index);
  mPattern.flip(index);
}

std::vector<MusicalEvent> RhythmPattern::asMusicalEvents(bool cyclic, bool trimDurationsToBeat) const
{
  std::vector<MusicalEvent> events;
  events.reserve(mNSteps);

  NaturalDurationsList durPool;
  const auto& naturalDurMap = mTs.getNaturalDurationsMap(
    mStepUnit, trimDurationsToBeat, &durPool);
  assert(naturalDurMap.size() == mNSteps);
  assert(durPool.back() == 1);

  size_t lo = 0;
  size_t hi = 0;
  MusicalEventType eTypePrev = MusicalEventType::REST;

  while ((int)(lo = hi) < mNSteps) {
    auto revDurIter = durPool.rbegin(); // starts at dur 1
    const size_t naturalDur = naturalDurMap[lo];
    const size_t maxEventEndPos = lo + naturalDur;

    // move hi by largest duration from pool without crossing an onset
    for (size_t tmpHi = lo + 1; tmpHi <= maxEventEndPos; ++tmpHi) {

      // it may get to rend() but then s <= maxEventEndPos will be false and 
      // the program shouldn't reach this iteration (in the for-loop)
      assert(revDurIter != durPool.rend());

      // move hi only if it results in a valid natural duration
      if (tmpHi - lo == *revDurIter) {
        hi = tmpHi;
        ++revDurIter;
      }

      // break if next step contains an onset
      if (mPattern[tmpHi % mNSteps]) break;
    }

    // note if onset. if no onset, tied note if preceded with onset; rest otherwise
    const MusicalEventType eTypeCurr = mPattern[lo]
      ? MusicalEventType::NOTE
      : (eTypePrev == MusicalEventType::NOTE
        ? MusicalEventType::TIED_NOTE
        : MusicalEventType::REST);

    events.push_back({ eTypeCurr, (int)lo, (int)(hi - lo) });
    eTypePrev = eTypeCurr;
  }

  // convert heading rest to tied note in cyclic
  if (cyclic) {
    const MusicalEvent& last = events.back();
    MusicalEvent& first = events.front();
    
    if (first.type == MusicalEventType::REST && last.type != MusicalEventType::REST)
      first.type = MusicalEventType::TIED_NOTE;
  }

  events.shrink_to_fit();
  return events;
}

void RhythmPattern::reset()
{
  reset(mTs, mStepUnit);
}

void RhythmPattern::reset(const TimeSignature& ts)
{
  reset(ts, mStepUnit);
}

void RhythmPattern::reset(UnitRef stepUnit)
{
  reset(mTs, stepUnit);
}

void RhythmPattern::reset(const TimeSignature & ts, UnitRef stepUnit)
{
  ts.checkStepUnit(stepUnit);
  mTs = ts;
  mStepUnit = stepUnit;
  updateNSteps();
  mPattern.reset();
}

void RhythmPattern::update(PatternId pattern)
{
  mPattern = std::bitset<MAX_N_STEPS>(pattern);
}

void RhythmPattern::checkStepIndex(size_t index) const
{
  if (index < 0 || index >= (size_t)mNSteps) {
    char msg[80];
    sprintf_s(msg, "expected index in range [0, %d] but got %d", mNSteps, index);
    throw std::out_of_range(msg);
  }
}

void RhythmPattern::checkNSteps() const
{
  if (mNSteps > MAX_N_STEPS) {
    char msg[70];
    sprintf_s(msg, "n steps %d exceeds maximum of %d", mNSteps, MAX_N_STEPS);
    throw std::runtime_error(msg);
  }
}

void RhythmPattern::updateNSteps()
{
  mNSteps = mTs.getExactMeasureDuration(mStepUnit);
  checkNSteps();
}
