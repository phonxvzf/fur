#ifndef MATH_UTIL_HPP
#define MATH_UTIL_HPP

#include "vector.hpp"

#define MATH_PI (3.141592653589793)

namespace math {

  extern const Float TWO_PI;
  extern const Float FOUR_PI;
  extern const Float INV_PI;
  extern const Float INV_TWO_PI;
  extern const Float INV_FOUR_PI;
  extern const Float PI_OVER_TWO;
  extern const Float PI_OVER_FOUR;

  inline Float max0(Float x) {
    return std::max(Float(0), x);
  }

  inline Float maxtol(Float x) {
    return std::max(FLOAT_TOLERANT, x);
  }

  inline Float mintol(Float x) {
    return std::min(ONE_MINUS_FLOAT_TOLERANT, x);
  }

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

  inline Float chi_plus(Float x) {
    return x > 0;
  }

  inline Float pow2(Float x) {
    return x * x;
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

  inline Float absdot(const vector3f& u, const vector3f& v) {
    return std::abs(u.dot(v));
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
    return (x * v.x) + (y * v.y) + (z * v.z);
  }

  inline vector3f reflect(const vector3f& omega, const normal3f& normal) {
    return 2 * omega.dot(normal) * normal - omega;
  }

  inline Float reduce_angle(Float theta) {
    while (theta > TWO_PI) theta -= TWO_PI;
    while (theta < 0) theta += TWO_PI;
    return theta;
  }

  /*
   * Find sin(x) from cos(x) where reduce_angle(x) is in range [0,pi]
   */
  inline Float sin_from_cos(Float cosine) {
    return std::sqrt(1 - mintol(pow2(cosine)));
  }

  inline Float sin_from_cos_theta(Float cosine, Float theta) {
    theta = reduce_angle(theta);
    if (theta >= 0 && theta <= MATH_PI) return sin_from_cos(cosine);
    return -sin_from_cos(cosine);
  }
}

#endif /* MATH_UTIL_HPP */
