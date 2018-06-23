#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "MTVRhythmSpace.h"

// TODO DRY
template <int N>
static PatternId createPattern(const std::string& str)
{
  // LSB in pattern id represents first step, therefore we reverse the str
  std::string strRev(str.rbegin(), str.rend());
  std::bitset<N> patternBitset(strRev, 0, strRev.size(), '-', 'x');
  return patternBitset.to_ullong();
}

TEST(MTVRhythmSpaceTests, StartsNotReady)
{
  MTVRhythmSpace s;
  ASSERT_FALSE(s.ready());
}

TEST(MTVRhythmSpaceTests, Dimensions)
{
  MTVRhythmSpace s(TimeSignature(6, 8), Unit::SEMIQUAVER);
  ASSERT_EQ(12, s.getDimensions());
}

TEST(MTVRhythmSpaceTests, RhythmCount)
{
  MTVRhythmSpace s(TimeSignature(6, 8), Unit::SEMIQUAVER);
  ASSERT_EQ(4096 /* 2 ^ 12 */, s.getPatternCount());
}

TEST(MTVRhythmSpaceTests, ReadyAfterFill)
{
  MTVRhythmSpace s(TimeSignature(1, 4), Unit::CROTCHET);  // timesig of 1/4 for fast fill()
  s.fill();
  ASSERT_TRUE(s.ready());
}

TEST(MTVRhythmSpaceTests, ComputeMTV)
{
  PatternId patternId = createPattern<16>("x--x---x--x-x---");
  RhythmPattern rhythm(TimeSignature(4, 4), Unit::SEMIQUAVER, patternId);
  MetricalSalienceProfile prf = { 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0 };

  Tension expectedMtv[16] = {
    0.00f, 0.00f, 0.00f, 1.00f, 
    1.00f, 1.00f, 0.75f, 1.00f,
    1.00f, 1.00f, 0.75f, 0.75f, 
    0.50f, 0.50f, 0.50f, 0.50f
  };

  Tension actualMtv[16];
  computeMTV(rhythm, prf, {0, 4}, actualMtv);
  ASSERT_THAT(actualMtv, ::testing::ElementsAreArray(expectedMtv));
}

TEST(MTVRhythmSpaceTests, FillAndGetMTV)
{
  TimeSignature ts(2, 4);
  UnitRef u = Unit::CROTCHET;
  MetricalSalienceProfile prf = { 1, 0 };

  PatternId pId00 = createPattern<16>("--");
  PatternId pId01 = createPattern<16>("-x");
  PatternId pId10 = createPattern<16>("x-");
  PatternId pId11 = createPattern<16>("xx");

  RhythmPattern rp00 = RhythmPattern(ts, u, pId00);
  RhythmPattern rp01 = RhythmPattern(ts, u, pId01);
  RhythmPattern rp10 = RhythmPattern(ts, u, pId10);
  RhythmPattern rp11 = RhythmPattern(ts, u, pId11);

  Tension eMtv00[2], eMtv01[2], eMtv10[2], eMtv11[2];
  computeMTV(rp00, prf, { 0, 1 }, eMtv00);
  computeMTV(rp01, prf, { 0, 1 }, eMtv01);
  computeMTV(rp10, prf, { 0, 1 }, eMtv10);
  computeMTV(rp11, prf, { 0, 1 }, eMtv11);

  MTVRhythmSpace s(TimeSignature(2, 4), Unit::CROTCHET);
  s.fill();

  std::vector<Tension> aMtv00(2), aMtv01(2), aMtv10(2), aMtv11(2);
  aMtv00.assign(s.getMTV(pId00), s.getMTV(pId00) + 2);
  aMtv01.assign(s.getMTV(pId01), s.getMTV(pId01) + 2);
  aMtv10.assign(s.getMTV(pId10), s.getMTV(pId10) + 2);
  aMtv11.assign(s.getMTV(pId11), s.getMTV(pId11) + 2);

  ASSERT_THAT(eMtv00, ::testing::ElementsAreArray(aMtv00));
  ASSERT_THAT(eMtv01, ::testing::ElementsAreArray(aMtv01));
  ASSERT_THAT(eMtv10, ::testing::ElementsAreArray(aMtv10));
  ASSERT_THAT(eMtv11, ::testing::ElementsAreArray(aMtv11));
}
