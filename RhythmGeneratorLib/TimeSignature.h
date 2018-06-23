#pragma once

#include "Unit.h"
#include <vector>
#include <bitset>
#include <assert.h>
#include <functional>

typedef int8_t MetricalSalience;
typedef std::vector<uint8_t> MeterSubdivisionList;
typedef std::vector<MetricalSalience> MetricalSalienceProfile;
typedef std::pair<MetricalSalience, MetricalSalience> MetricalSalienceRange;
typedef std::vector<int8_t> NaturalDurationsList;


struct MeterTreeNodeInfo
{
  int depth      = 0;
  int nSiblings  = 1;
  int nNodesOnSameLevel = 1;
  int treeWidth  = 1;
  int treeHeight = 1;

  MeterTreeNodeInfo(int depth, int nSiblings, int nNodesOnSameLevel, int treeWidth, int treeHeight) : 
    depth(depth), nSiblings(nSiblings), nNodesOnSameLevel(nNodesOnSameLevel), treeWidth(treeWidth), treeHeight(treeHeight) {}
};

class TimeSignature
{
public:
  TimeSignature();
  TimeSignature(int numerator, int denominator);
  virtual ~TimeSignature() {}

  inline int getNumerator() const { return mNumerator; }
  inline int getDenominator() const { return mDenominator; }
  inline UnitRef getBeatUnit() const { return mBeatUnit; }

  double getMeasureDuration(UnitRef unit) const;
  int getExactMeasureDuration(UnitRef unit) const;

  MeterSubdivisionList getHierarchicalMeterSubdivisions(UnitRef stepUnit) const;
  MetricalSalienceProfile getMetricalSalienceProfile(UnitRef stepUnit, int rootWeight = 0) const;
  NaturalDurationsList getNaturalDurationsMap(UnitRef stepUnit, bool trimToBeat = true, 
    NaturalDurationsList* naturalDurationsOut = nullptr) const;

  void checkStepUnit(UnitRef unit) const;

  inline bool operator==(const TimeSignature &other) const 
  {
    return mNumerator == other.mNumerator && mDenominator == other.mDenominator;
  }

protected:
  template <typename T>
  inline std::vector<T> createMeterMap(
    UnitRef stepUnit,
    std::function<T(const MeterTreeNodeInfo& node)> getValue,
    std::vector<T>* valuesPerLevelOut = nullptr
  ) const
  {
    MeterSubdivisionList subdivisions = getHierarchicalMeterSubdivisions(stepUnit);
    const int nSteps = getExactMeasureDuration(stepUnit);
    const int nLevels = subdivisions.size() + 1;

    std::vector<T> meterMap(nSteps);
    std::vector<bool> alreadySetStepIndices(nSteps);

    MeterTreeNodeInfo nodeInfo(0, 1, 1, nSteps, nLevels);
    meterMap[0] = getValue(nodeInfo);
    alreadySetStepIndices[0] = true;

    if (valuesPerLevelOut) {
      assert(valuesPerLevelOut->empty());
      valuesPerLevelOut->resize(nLevels);
      valuesPerLevelOut->at(0) = meterMap[0];
    }

    if (subdivisions.empty()) {
      assert(nSteps == 1);
      return meterMap;
    }

    T value;
    int nStepsCurrNode;

    for (const auto& sub : subdivisions) {
      ++nodeInfo.depth;
      nodeInfo.nSiblings = sub;
      nodeInfo.nNodesOnSameLevel *= sub;
      nStepsCurrNode = nSteps / nodeInfo.nNodesOnSameLevel;
      value = getValue(nodeInfo);

      if (valuesPerLevelOut)
        valuesPerLevelOut->at(nodeInfo.depth) = value;

      // start from nStepsCurrNode, because ix 0 is already set
      for (int stepIx = nStepsCurrNode; stepIx < nSteps; stepIx += nStepsCurrNode) {
        if (alreadySetStepIndices[stepIx]) continue;
        meterMap[stepIx] = value;
        alreadySetStepIndices[stepIx] = true;
      }
    }

    meterMap.shrink_to_fit();
    return meterMap;
  }

private:
  int mNumerator;
  int mDenominator;
  UnitRef mBeatUnit;
};
