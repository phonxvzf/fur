#ifndef MATH_FLOAT_HPP
#define MATH_FLOAT_HPP

#include <cmath>

#ifdef USE_DOUBLE_AS_FLOAT
  typedef double Float;
#else
  typedef float Float;
#endif

#define FLOAT_TOLERANT (1e-6f)

inline bool COMPARE_EQ(Float x, Float y) {
  return std::abs(x - y) < FLOAT_TOLERANT;
}

#endif /* MATH_FLOAT_HPP */
