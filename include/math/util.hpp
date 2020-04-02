#ifndef MATH_UTIL_HPP
#define MATH_UTIL_HPP

#include "vector.hpp"

#define MATH_PI (3.141592653589793)
#define INSIDE  (0)
#define OUTSIDE (1)

namespace math {

  extern const Float PI;
  extern const Float TWO_PI;
  extern const Float FOUR_PI;
  extern const Float INV_PI;
  extern const Float INV_TWO_PI;
  extern const Float INV_FOUR_PI;
  extern const Float INV_EIGHT_PI;
  extern const Float PI_OVER_TWO;
  extern const Float PI_OVER_FOUR;
  extern const Float SQRT_TWO;
  extern const Float ONE_OVER_THREE;

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
      return COMPARE_LEQ(a, x) && COMPARE_LEQ(x, b);
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

  inline Float pow3(Float x) {
    return x * x * x;
  }

  inline Float pow4(Float x) {
    return (x * x) * (x * x);
  }

  inline Float pow5(Float x) {
    return pow4(x) * x;
  }

  inline Float pow20(Float x) {
    Float x2 = pow2(x);
    Float x4 = pow2(x2);
    Float x8 = pow2(x4);
    return pow2(x8) * x4;
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

  inline Float dotproj(const vector3f& u, const vector3f& v) {
    return u.x * v.x + u.y * v.y;
  }

  inline vector3f left_to_right(const vector3f& v) {
    return { -v.z, v.x, v.y };
  }

  inline vector3f right_to_left(const vector3f& v) {
    return { v.y, v.z, -v.x };
  }

  inline vector3f spherical_coords(Float theta, Float phi) {
    Float sin_theta = std::sin(theta);
    return right_to_left(
        { sin_theta * std::cos(phi), sin_theta * std::sin(phi), std::cos(theta) }
        );
  }

  inline vector3f zup_to_yup(const vector3f& v) {
    return { v.x, v.z, v.y };
  }

  inline vector3f clear_neg_zero(const vector3f& v) {
    return {
      COMPARE_EQ(v.x, 0) ? Float(0) : v.x,
      COMPARE_EQ(v.y, 0) ? Float(0) : v.y,
      COMPARE_EQ(v.z, 0) ? Float(0) : v.z,
    };
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

  /*
   * Find cos(x) from sin(x)
   */
  inline Float cos_from_sin(Float sine) {
    return std::sqrt(1 - mintol(pow2(sine)));
  }

  // Compute Fresnel term (exact version from Cook-Torrance paper)
  inline Float fresnel(
      const vector3f& omega,
      const normal3f& mf_normal,
      Float eta_i,
      Float eta_t)
  {
    const Float c = absdot(omega, mf_normal);
    const Float g2 = pow2(eta_t / eta_i) - 1 + pow2(c);

    if (g2 < 0) return 1; // total internal reflection

    const Float g = std::sqrt(g2);

    return 0.5f * pow2((g - c) / (g + c))
      * (1 + pow2((c * (g + c) - 1) / (c * (g - c) + 1)));
  }

  // Compute Fresnel term (exact version from Cook-Torrance paper), cosine signature
  inline Float fresnel_cosine(Float c, Float eta_i, Float eta_t) {
    if (c < 0) {
      std::swap(eta_i, eta_t);
      c = std::abs(c);
    }
    const Float g2 = pow2(eta_t / eta_i) - 1 + pow2(c);

    if (g2 < 0) return 1; // total internal reflection

    const Float g = std::sqrt(g2);

    return 0.5f * pow2((g - c) / (g + c))
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

  // Compute average diffuse Fresnel reflectance Fdr
  // eta is relative index of refraction
  inline Float fresnel_diffuse(Float eta) {
    return -1.440f / pow2(eta) + 0.710f / eta + 0.668f + 0.0636f * eta;
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

  inline Float asin_clamp(Float x) {
    return std::asin(math::clamp(x, -1.f, 1.f));
  }

  inline Float mitchell(Float B, Float C, Float x) {
    Float ax = std::abs(x);
    Float f = 0;
    if (ax < 1) {
      f = (12 - 9 * B - 6 * C) * pow3(ax) + (-18 + 12 * B + 6 * C) * pow2(ax) + (6 - 2 * B);
    } else if (ax < 2) {
      f = (-B - 6 * C) * pow3(ax) + (6 * B + 30 * C) * pow2(ax) + (-12 * B - 48 * C) * ax
        + (8 * B + 24 * C);
    }
    return 1.f / 6 * f;
  }

  inline Float mitchell_filter(Float B, Float C, Float x) {
    // scale x from range [-1,1] to [-2,2]
    return mitchell(B, C, 2.f * x);
  }

  inline Float triangle_filter(Float x) {
    return std::max(x >= 0.f ? 1.f - x : x + 1.f, 0.f);
  }

  inline Float blackman_harris_filter(Float x) {
    // x is scaled from range [-1,1] to [-2,2] and canceled with z = 2 
    x = x * 0.5f + 1.f;
    return 0.35875f - 0.48829f * std::cos(PI * x)
      + 0.14128f * std::cos(TWO_PI * x)
      - 0.01168f * std::cos(3.f * PI * x);
  }

  inline Float gaussian_filter(Float x) {
    return std::exp(-0.5f * pow2(2.f * x));
  }

  inline int log2_nearest(Float x) {
    if (x < 1) return 0;
    return 32 - __builtin_clz(static_cast<uint32_t>(x));
  }

  inline int log4_nearest(Float x) {
    return log2_nearest(x) >> 1;
  }

  inline Float balance_heuristic(size_t n0, Float p0, size_t n1, Float p1) {
    Float w0 = n0 * p0;
    Float w1 = n1 * p1;
    return pow2(w0) / (pow2(w0) + pow2(w1));
  }

  // Code below are from pbrt
  inline uint32_t compact_1by1(uint32_t x) {
    x &= 0x55555555;
    x = (x ^ (x >> 1)) & 0x33333333;
    x = (x ^ (x >> 2)) & 0x0f0f0f0f;
    x = (x ^ (x >> 4)) & 0x00ff00ff;
    x = (x ^ (x >> 8)) & 0x0000ffff;
    return x;
  }

  inline point2f demux_float(Float x) {
    uint64_t v = x * (1ULL << 32);
    uint32_t bits[2] = { compact_1by1(v), compact_1by1(v >> 1) };
    return { bits[0] / Float(1 << 16), bits[1] / Float(1 << 16) };
  }
}

#endif /* MATH_UTIL_HPP */
