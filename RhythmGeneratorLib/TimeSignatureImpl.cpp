#include "TimeSignatureImpl.h"
#include <stdexcept>

TimeSignatureImpl::TimeSignatureImpl() : TimeSignatureImpl(4, 4)
{
}

TimeSignatureImpl::TimeSignatureImpl(int numerator, int denominator) : 
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

TimeSignatureImpl::~TimeSignatureImpl()
{
}

double TimeSignatureImpl::getMeasureDuration(UnitRef unit) const
{
  return mBeatUnit->convert(mNumerator, unit);
}

int TimeSignatureImpl::getExactMeasureDuration(UnitRef unit) const
{
  return mBeatUnit->convertExact(mNumerator, unit);
}
