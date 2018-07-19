#pragma once

#include "gmock/gmock.h"
#include "TimeSignature.h"

class MockTimeSignature : public TimeSignature {
public:
  MOCK_CONST_METHOD0(getNumerator, int());
  MOCK_CONST_METHOD0(getDenominator, int());
  MOCK_CONST_METHOD1(getMeasureDuration, double(UnitRef unit));
  MOCK_CONST_METHOD1(getExactMeasureDuration, int(UnitRef unit));
};