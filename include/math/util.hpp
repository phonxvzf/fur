#ifndef MATH_UTIL_HPP
#define MATH_UTIL_HPP

#include "vector.hpp"

#define MATH_PI (3.141592653589793)

namespace math {
  inline Float radians(Float deg) {
    return MATH_PI / 180.0 * deg;
  }

  inline Float degrees(Float rad) {
    return 180.0 / MATH_PI * rad;
  }

  inline Float sign(Float x) {
    if (COMPARE_EQ(x, 0)) return 0;
    return x > 0 ? 1 : -1;
  }

  inline Float dot2(const vector2f v) {
    return v.dot(v);
  }

  inline Float dot2(const vector3f v) {
    return v.dot(v);
  }

  inline Float dot2(const vector4f v) {
    return v.dot(v);
  }

  extern const Float TWO_PI;
  extern const Float FOUR_PI;
  extern const Float INV_PI;
  extern const Float INV_TWO_PI;
  extern const Float INV_FOUR_PI;
  extern const Float PI_OVER_TWO;
  extern const Float PI_OVER_FOUR;
}

#endif /* MATH_UTIL_HPP */
