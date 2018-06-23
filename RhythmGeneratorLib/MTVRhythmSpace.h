#pragma once

#include "Types.h"
#include "RhythmPattern.h"
#include "TimeSignature.h"
#include <mutex>
#include <future>
#include <vector>
#include <random>
#include <functional>

#define MTV_RHYTHM_SPACE_RAND_SIGMA 0.0002


class MTVRhythmSpace
{
public: 
  MTVRhythmSpace(
    const TimeSignature& ts = TimeSignature(4, 4),
    UnitRef stepUnit = Unit::QUAVER
  );

  virtual ~MTVRhythmSpace();

  // computes and fills this space with mtv points for all possible rhythm 
  // patterns with this space's time signature and step unit
  void fill(std::function<void(double)> progressFuncCallback = nullptr);

  inline bool ready() const { return mReady; }  // returns whether fill() has already been called
  const Tension * const getMTV(const PatternId) const; // returns the MTV for the given rhythm or nullptr
  int getPatternCount() const;  // returns the number of rhythms in this space
  int getDimensions() const;  // returns the number of dimensions
  PatternId getClosestPattern(const Tension * const mtv); // returns the pattern whose mtv is closest to the given point
  PatternId getRandomPatternCloseTo(const Tension * const mtv);

protected:
  void checkIfReady() const;
  void updateDistanceCache(const Tension * const mtv);
  bool equalsDistanceCacheTargetPoint(const Tension * const mtv) const;

private:
  bool mReady;
  const TimeSignature mTs;
  const UnitRef mStepUnit;
  const int mNSteps;
  const int mNPoints;  // order is important (must go after mNSteps)
  std::vector<Tension*> mPoints;
  std::mt19937 mRandom;
  std::normal_distribution<double> mNormDist;  // order is import (must go after mNPoints)
  typedef std::pair<float, PatternId> DistanceCacheEntry;
  std::vector<DistanceCacheEntry> mDistanceCache;
  Tension * mDistanceCacheTargetPoint;
};

// Computes the metrical tension vector of a rhythm pattern, given a metrical salience profile and 
// the min-max values of that salience profile. The given Tension pointer should point to an array 
// of rhythm.getNSteps() elements (otherwise, calling this function results in undefined behaviour).
void computeMTV(
  const RhythmPattern& rhythm,
  const MetricalSalienceProfile& prf,
  MetricalSalienceRange salienceRange,
  Tension * mtvOut
);
