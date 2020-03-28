#ifndef MATH_FLOAT_HPP
#define MATH_FLOAT_HPP

#include <cmath>
#include <algorithm>

#ifdef USE_DOUBLE_AS_FLOAT
  #error Using double precision float is currently not supported.
  typedef double Float;
#else
  typedef float Float;
#endif

extern const Float FLOAT_TOLERANT;
extern const Float ONE_MINUS_FLOAT_TOLERANT;

namespace math {
  inline bool COMPARE_EQ(Float x, Float y) {
    return std::abs(x - y) < FLOAT_TOLERANT;
  }

  inline bool COMPARE_EQ(Float x, Float y, Float eps) {
    return std::abs(x - y) < eps;
  }

  inline bool COMPARE_LEQ(Float x, Float y) {
    return COMPARE_EQ(x, y) || (x < y);
  }

  inline bool COMPARE_GEQ(Float x, Float y) {
    return COMPARE_EQ(x, y) || (x > y);
  }

  inline Float lerp(Float t, Float a, Float b) {
    return (1 - t) * a + t * b;
  }

  template <typename T>
    inline T clamp(T x, T min, T max) {
      return std::min(max, std::max(min, x));
    }
}

#endif /* MATH_FLOAT_HPP */
