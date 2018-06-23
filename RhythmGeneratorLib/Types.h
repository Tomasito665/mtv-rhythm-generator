#pragma once

#include <bitset>

typedef float Tension;
typedef Tension * const MTV;
typedef const Tension * const ConstMTV;
typedef uint64_t PatternId;
const int MAX_N_STEPS = sizeof(PatternId) * CHAR_BIT;
#define EMPTY_RHYTHM_PATTERN 0ULL
