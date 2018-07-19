#include "gtest/gtest.h"
#include "RhythmPattern.h"
#include <bitset>

template <int N>
static PatternId createPattern(const std::string& str)
{
  // LSB in pattern id represents first step, therefore we reverse the str
  std::string strRev(str.rbegin(), str.rend());
  std::bitset<N> patternBitset(strRev, 0, strRev.size(), '-', 'x');
  return patternBitset.to_ullong();
}


TEST(RhythmPatternTests, StepCount)
{
  RhythmPattern r(TimeSignature(6, 8), Unit::SEMIQUAVER);
  ASSERT_EQ(12, r.getNSteps());
}

TEST(RhythmPatternTests, StartsEmpty)
{
  RhythmPattern r;
  const int N = r.getNSteps();
  for (int i = 0; i < N; ++i)
    ASSERT_FALSE(r[i]);
}

TEST(RhythmPatternTests, ConstructorPatternId)
{
  PatternId patternId = createPattern<8>("xx-x-xx-");
  RhythmPattern r(TimeSignature(4, 4), Unit::QUAVER, patternId);

  std::vector<int> expectedPattern = { 1, 1, 0, 1, 0, 1, 1, 0 };
  std::vector<int> actualPattern(8);

  for (int i = 0; i < 8; ++i)
    actualPattern[i] = (int)r[i];

  ASSERT_EQ(expectedPattern, actualPattern);
}

TEST(RhythmPatternTests, AsMusicalEvents)
{
  PatternId patternId = createPattern<16>("x--x---x--x-x---");  // rumba clave
  RhythmPattern r(TimeSignature(4, 4), Unit::SEMIQUAVER, patternId);
  
  MusicalEventList expectedEvents = {
    MusicalEvent(MusicalEventType::NOTE, 0, 2),
    MusicalEvent(MusicalEventType::TIED_NOTE, 2, 1),
    MusicalEvent(MusicalEventType::NOTE, 3, 1),
    MusicalEvent(MusicalEventType::TIED_NOTE, 4, 2),
    MusicalEvent(MusicalEventType::REST, 6, 1),
    MusicalEvent(MusicalEventType::NOTE, 7, 1),
    MusicalEvent(MusicalEventType::TIED_NOTE, 8, 2),
    MusicalEvent(MusicalEventType::NOTE, 10, 2),
    MusicalEvent(MusicalEventType::NOTE, 12, 4)
  };

  MusicalEventList actualEvents = r.asMusicalEvents(false, false);
  ASSERT_EQ(expectedEvents, actualEvents);
}

TEST(RhythmPatternTests, AsMusicalEventsCyclicTrim)
{
  PatternId patternId = createPattern<4>("---x");  // rumba clave
  RhythmPattern r(TimeSignature(4, 4), Unit::CROTCHET, patternId);

  MusicalEventList expectedEvents = {
    MusicalEvent(MusicalEventType::TIED_NOTE, 0, 1),
    MusicalEvent(MusicalEventType::REST, 1, 1),
    MusicalEvent(MusicalEventType::REST, 2, 1),
    MusicalEvent(MusicalEventType::NOTE, 3, 1)
  };

  MusicalEventList actualEvents = r.asMusicalEvents(true, true);
  ASSERT_EQ(expectedEvents, actualEvents);
}

TEST(RhythmPatternTests, AsMusicalEventsNoCyclicTrim)
{
  PatternId patternId = createPattern<4>("---x");  // rumba clave
  RhythmPattern r(TimeSignature(4, 4), Unit::CROTCHET, patternId);

  MusicalEventList expectedEvents = {
    MusicalEvent(MusicalEventType::REST, 0, 1),
    MusicalEvent(MusicalEventType::REST, 1, 1),
    MusicalEvent(MusicalEventType::REST, 2, 1),
    MusicalEvent(MusicalEventType::NOTE, 3, 1)
  };

  MusicalEventList actualEvents = r.asMusicalEvents(false, true);
  ASSERT_EQ(expectedEvents, actualEvents);
}

TEST(RhythmPatternTests, AsMusicalEventsCyclicNoTrim)
{
  PatternId patternId = createPattern<4>("---x");  // rumba clave
  RhythmPattern r(TimeSignature(4, 4), Unit::CROTCHET, patternId);

  MusicalEventList expectedEvents = {
    MusicalEvent(MusicalEventType::TIED_NOTE, 0, 2),
    MusicalEvent(MusicalEventType::REST, 2, 1),
    MusicalEvent(MusicalEventType::NOTE, 3, 1)
  };

  MusicalEventList actualEvents = r.asMusicalEvents(true, false);
  ASSERT_EQ(expectedEvents, actualEvents);
}

TEST(RhythmPatternTests, AsMusicalEventsNoCyclicNoTrim)
{
  PatternId patternId = createPattern<4>("---x");  // rumba clave
  RhythmPattern r(TimeSignature(4, 4), Unit::CROTCHET, patternId);

  MusicalEventList expectedEvents = {
    MusicalEvent(MusicalEventType::REST, 0, 2),
    MusicalEvent(MusicalEventType::REST, 2, 1),
    MusicalEvent(MusicalEventType::NOTE, 3, 1)
  };

  MusicalEventList actualEvents = r.asMusicalEvents(false, false);
  ASSERT_EQ(expectedEvents, actualEvents);
}
