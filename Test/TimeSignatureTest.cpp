#include "gtest/gtest.h"
#include "TimeSignature.h"
#include "Unit.h"
#include <stdexcept>
#include <vector>

TEST(TimeSignatureTests, DefaultsToFourFourth)
{
  TimeSignature ts;
  ASSERT_EQ(4, ts.getNumerator());
  ASSERT_EQ(4, ts.getDenominator());
}

TEST(TimeSignatureTests, CrashesOnOddDenominator)
{
  ASSERT_NO_THROW(TimeSignature(1, 2));
  ASSERT_ANY_THROW(TimeSignature(1, 3));
}

TEST(TimeSignatureTests, ExactMeasureDuration)
{
  TimeSignature ts(6, 8);
  ASSERT_EQ(12, ts.getExactMeasureDuration(Unit::SEMIQUAVER));
}

TEST(TimeSignatureTests, MeasureDuration)
{
  TimeSignature ts(6, 8);
  ASSERT_DOUBLE_EQ(1.5, ts.getMeasureDuration(Unit::MINIM));
}

TEST(TimeSignatureTests, BeatUnit)
{
  TimeSignature ts(6, 8);
  ASSERT_EQ(Unit::QUAVER, ts.getBeatUnit());
}

TEST(TimeSignatureTests, HierarchicalMeterSubdivisionsSixEighth)
{
  TimeSignature ts(6, 8);
  MeterSubdivisionList expectedSubdivisions = { 2, 3, 2 };
  ASSERT_EQ(expectedSubdivisions, ts.getHierarchicalMeterSubdivisions(Unit::SEMIQUAVER));
}

TEST(TimeSignatureTests, HierarchicalMeterSubdivisionsThreeFourth)
{
  TimeSignature ts(3, 4);
  MeterSubdivisionList expectedSubdivisions = { 3, 2, 2 };
  ASSERT_EQ(expectedSubdivisions, ts.getHierarchicalMeterSubdivisions(Unit::SEMIQUAVER));
}

TEST(TimeSignatureTests, MetricalSalienceProfileSixEighth)
{
  TimeSignature ts(6, 8);
  MetricalSalienceProfile expectedPrf = { 10, 7, 8, 7, 8, 7, 9, 7, 8, 7, 8, 7 };
  ASSERT_EQ(expectedPrf, ts.getMetricalSalienceProfile(Unit::SEMIQUAVER, 10));
}

TEST(TimeSignatureTests, MetricalSalienceProfileThreeFourth)
{
  TimeSignature ts(3, 4);
  MetricalSalienceProfile expectedPrf = { 10, 7, 8, 7, 9, 7, 8, 7, 9, 7, 8, 7 };
  ASSERT_EQ(expectedPrf, ts.getMetricalSalienceProfile(Unit::SEMIQUAVER, 10));
}

TEST(TimeSignatureTests, MetricalSalienceProfileWithNoSubdivisions)
{
  TimeSignature ts(1, 4);
  MetricalSalienceProfile expectedPrf = { 10 };
  ASSERT_EQ(expectedPrf, ts.getMetricalSalienceProfile(Unit::CROTCHET, 10));
}

TEST(TimeSignatureTests, NaturalDurationsMapNoTrim)
{
  TimeSignature ts(6, 8);
  NaturalDurationsList durationsPool;
  NaturalDurationsList expectedMap = { 12, 1, 2, 1, 2, 1, 6, 1, 2, 1, 2, 1 };
  NaturalDurationsList expectedDurationsPerLevel = { 12, 6, 2, 1 };
  ASSERT_EQ(expectedMap, ts.getNaturalDurationsMap(Unit::SEMIQUAVER, false, &durationsPool));
  ASSERT_EQ(expectedDurationsPerLevel, durationsPool);
}

TEST(TimeSignatureTests, NaturalDurationsMapWithTrim)
{
  TimeSignature ts(6, 8);
  NaturalDurationsList durationsPerLevel;
  NaturalDurationsList expectedDurationsPerLevel = { 2, 1 };
  NaturalDurationsList expectedMap = { 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1 };
  ASSERT_EQ(expectedMap, ts.getNaturalDurationsMap(Unit::SEMIQUAVER, true, &durationsPerLevel));
  ASSERT_EQ(expectedDurationsPerLevel, durationsPerLevel);
}

int main(int argc, char* argv[])
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
