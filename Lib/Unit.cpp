#include "Unit.h"
#include <assert.h>

const UnitRef Unit::SEMIQUAVER = UnitRef(new Unit(16));
const UnitRef Unit::QUAVER = UnitRef(new Unit(8));
const UnitRef Unit::CROTCHET = UnitRef(new Unit(4));
const UnitRef Unit::MINIM = UnitRef(new Unit(2));
const UnitRef Unit::SEMIBREVE = UnitRef(new Unit(1));

Unit::Unit(int numerator, int denominator) : 
  mNumerator(numerator),
  mDenominator(denominator),
  mInAtoms(UNIT_ATOM_SIZE / denominator * numerator)
{
    assert(UNIT_ATOM_SIZE % denominator == 0);
}

Unit::Unit(int denominator) : Unit(1, denominator)
{
}

UnitRef Unit::get(int numerator, int denominator)
{
  if (numerator != 1 || denominator > UNIT_ATOM_SIZE) {
    return UnitRef(nullptr);
  }

  switch (denominator)
  {
  case 16:
    return Unit::SEMIQUAVER;
  case 8:
    return Unit::QUAVER;
  case 4:
    return Unit::CROTCHET;
  case 2:
    return Unit::MINIM;
  case 1:
    return Unit::SEMIBREVE;
  default:
    return UnitRef(nullptr);
  }
}

UnitRef Unit::get(int denominator)
{
  return Unit::get(1, denominator);
}

double Unit::convert(double value, UnitRef toUnit) const
{
  return value * (double)mInAtoms / toUnit->mInAtoms;
}

int Unit::convertExact(int value, UnitRef toUnit) const
{
  return value * mInAtoms / toUnit->mInAtoms;
}

bool Unit::operator==(const Unit & other) const
{
  return mInAtoms == other.mInAtoms;
}

bool Unit::operator<(const Unit& other) const
{
  return mInAtoms < other.mInAtoms;
}

bool Unit::operator>(const Unit& other) const
{
  return mInAtoms > other.mInAtoms;
}

bool Unit::operator<=(const Unit & other) const
{
  return mInAtoms <= other.mInAtoms;
}

bool Unit::operator>=(const Unit & other) const
{
  return other.mInAtoms >= other.mInAtoms;
}

