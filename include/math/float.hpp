#ifndef MATH_FLOAT_HPP
#define MATH_FLOAT_HPP

#include <cmath>
#include <algorithm>

#ifdef USE_DOUBLE_AS_FLOAT
  typedef double Float;
#else
  typedef float Float;
#endif

#define FLOAT_TOLERANT (1e-6)

namespace math {
  inline bool COMPARE_EQ(Float x, Float y) {
    return std::abs(x - y) < FLOAT_TOLERANT;
  }

  inline Float clamp(Float x, Float min, Float max) {
    return std::min(max, std::max(min, x));
  }

  inline Float lerp(Float t, Float a, Float b) {
    return (1 - t) * a + t * b;
  }
}

#endif /* MATH_FLOAT_HPP */
