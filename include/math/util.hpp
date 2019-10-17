#ifndef MATH_UTIL_HPP
#define MATH_UTIL_HPP

#include "float.hpp"

#define MATH_PI (3.141592653589793)

namespace math {
  inline Float radians(Float deg) {
    return MATH_PI / 180.0 * deg;
  }

  inline Float degrees(Float rad) {
    return 180.0 / MATH_PI * rad;
  }
}

#endif /* MATH_UTIL_HPP */
