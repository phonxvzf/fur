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

  inline Float pow4(Float x) {
    return (x * x) * (x * x);
  }

  inline Float pow5(Float x) {
    return pow4(x) * x;
  }

  inline Float dot2(const vector2f& v) {
    return v.dot(v);
  }

  inline Float dot2(const vector3f& v) {
    return v.dot(v);
  }

  inline Float dot2(const vector4f& v) {
    return v.dot(v);
  }

  inline Float maxdot(const vector3f& u, const vector3f& v) {
    return std::max(Float(0), u.dot(v));
  }

  inline vector3f left_to_right(const vector3f& v) {
    return { -v.z, v.x, v.y };
  }

  inline vector3f right_to_left(const vector3f& v) {
    return { v.y, v.z, -v.x };
  }

  inline vector3f change_bases(
      const vector3f& v,
      const vector3f& x,
      const vector3f& y,
      const vector3f& z)
  {
    return x * v.x + y * v.y + z * v.z;
  }

  inline vector3f reflect(const vector3f& omega, const normal3f& normal) {
    return 2 * maxdot(omega, normal) * normal - omega;
  }

  inline void orthogonals(const vector3f& n, vector3f* u, vector3f* v) {
    if (std::abs(n.x) > std::abs(n.y)) *u = vector3f(-n.z, 0, n.x).normalized();
    else *u = vector3f(0, n.z, -n.y).normalized();
    *v = n.cross(*u);
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
