#pragma once

#include <memory>

#define UNIT_ATOM_SIZE 128
typedef std::shared_ptr<const class Unit> UnitRef;


class Unit
{
public:
  static const UnitRef SEMIQUAVER;  // sixteenth
  static const UnitRef QUAVER;      // eighth
  static const UnitRef CROTCHET;    // quarter
  static const UnitRef MINIM;       // half
  static const UnitRef SEMIBREVE;   // whole

  static UnitRef get(int numerator, int denominator);
  static UnitRef get(int denominator);

  virtual ~Unit() {};

  inline int getNumerator() const { return mNumerator; }
  inline int getDenominator() const { return mDenominator; }

  double convert(double value, UnitRef toUnit) const;
  int convertExact(int value, UnitRef toUnit) const;

  bool operator==(const Unit& other) const;
  bool operator<(const Unit& other) const;
  bool operator>(const Unit& other) const;
  bool operator<=(const Unit& other) const;
  bool operator>=(const Unit& other) const;
  
protected:
  explicit Unit(int numerator, int denominator);
  explicit Unit(int denominator);

private:
  const int mNumerator;
  const int mDenominator;
  const int mInAtoms;
};
