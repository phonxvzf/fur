#ifndef MATH_UTIL_HPP
#define MATH_UTIL_HPP

#include "vector.hpp"

#define MATH_PI (3.141592653589793)
#define INSIDE  (0)
#define OUTSIDE (1)

namespace math {

  extern const Float TWO_PI;
  extern const Float FOUR_PI;
  extern const Float INV_PI;
  extern const Float INV_TWO_PI;
  extern const Float INV_FOUR_PI;
  extern const Float PI_OVER_TWO;
  extern const Float PI_OVER_FOUR;
  extern const Float SQRT_TWO;

  template <typename T>
    inline T max3(T a, T b, T c) {
      return std::max(a, std::max(b, c));
    }

  template <typename T>
    inline T min3(T a, T b, T c) {
      return std::min(a, std::min(b, c));
    }

  template <typename T>
    inline bool in_range(T x, T a, T b) {
      return (a <= x) && (x <= b);
    }

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
    return x < 0 ? -1 : 1;
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

  inline vector3f reflect(const vector3f& omega, const normal3f& normal) {
    return (2 * omega.dot(normal) * normal - omega).normalized();
  }

  inline vector3f refract(
      const vector3f& omega,
      const normal3f& normal,
      Float eta,
      bool* tir = nullptr
      )
  {
    Float cos_theta_i = omega.dot(normal);
    Float sin2_theta_i = max0(1 - cos_theta_i * cos_theta_i); // in case of floating point error
    Float sin2_theta_t = eta * eta * sin2_theta_i; // Snell's law
    if (COMPARE_GEQ(sin2_theta_t, 1)) {
      if (tir) *tir = true;
      return reflect(omega, normal);
    }
    if (tir) *tir = false;
    Float cos_theta_t = std::sqrt(1 - sin2_theta_t);
    return ((-omega * eta) + (eta * cos_theta_i - cos_theta_t) * normal).normalized();
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

  inline Float fresnel(
      const vector3f& omega,
      const normal3f& mf_normal,
      Float eta_i,
      Float eta_t)
  {
    // Exact version from Cook-Torrance paper
    const Float c = absdot(omega, mf_normal);
    const Float g2 = pow2(eta_t / eta_i) - 1 + pow2(c);

    if (g2 < 0) return 1; // total internal reflection

    const Float g = std::sqrt(g2);

    return Float(0.5) * pow2((g - c) / (g + c))
      * (1 + pow2((c * (g + c) - 1) / (c * (g - c) + 1)));
  }

  inline Float fresnel_schlick(
      const vector3f& omega,
      const normal3f& mf_normal,
      Float eta_i,
      Float eta_t)
  {
    const Float r0 = pow2((eta_i - eta_t) / (eta_i + eta_t));
    return r0 + (1 - r0) * pow5(1 - absdot(omega, mf_normal));
  }

  template <typename T>
    inline point3<T> minp3(const point3<T>& p1, const point3<T>& p2) {
      return {
        std::min(p1.x, p2.x),
        std::min(p1.y, p2.y),
        std::min(p1.z, p2.z)
      };
    }

  template <typename T>
    inline point3<T> maxp3(const point3<T>& p1, const point3<T>& p2) {
      return {
        std::max(p1.x, p2.x),
        std::max(p1.y, p2.y),
        std::max(p1.z, p2.z)
      };
    }

  inline Float sinc(Float x) {
    if (COMPARE_EQ(x, 0)) return 1;
    Float sine = std::sin(x);
    if (COMPARE_EQ(x, 0)) return 1;
    return sine / x;
  }

  inline Float solve_quadratic(Float* sol0, Float* sol1, Float a, Float b, Float c) {
    const Float delta = pow2(b) - 4 * a * c;
    if (delta < 0) return delta;
    const Float sqrt_delta = std::sqrt(delta);
    const Float inv_two_a = 1.f / (2 * a);
    *sol0 = (-b - sqrt_delta) * inv_two_a;
    *sol1 = (-b + sqrt_delta) * inv_two_a;
    return delta;
  }
}

#endif /* MATH_UTIL_HPP */
