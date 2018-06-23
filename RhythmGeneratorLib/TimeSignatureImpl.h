#pragma once

#include "TimeSignature.h"


class TimeSignatureImpl : public TimeSignature
{
public:
  TimeSignatureImpl();
  TimeSignatureImpl(int numerator, int denominator);
  virtual ~TimeSignatureImpl() override;

  inline int getNumerator() const override { return mNumerator; }
  inline int getDenominator() const override { return mDenominator; }
  double getMeasureDuration(UnitRef unit) const override;
  int getExactMeasureDuration(UnitRef unit) const override;

private:
  int mNumerator;
  int mDenominator;
  UnitRef mBeatUnit;
};
