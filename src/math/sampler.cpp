#include "math/sampler.hpp"
#include "math/util.hpp"
#include "error.hpp"

namespace math {
  namespace sampler {
    uniform1d::uniform1d(int n_samples, Float a, Float b)
      : pdf1d(n_samples, a, b)
    {
      ASSERT(!COMPARE_EQ(a, b) && (b > a));
      Float y = 1 / (b - a);
      for (int i = 0; i < n_samples; ++i) {
        pdf[i] = y;
      }
      init_cdf();
    }

    normal1d::normal1d(int n_samples, Float mean, Float var)
      : pdf1d(
          n_samples,
          mean - std::sqrt(-2 * var * std::log(FLOAT_TOLERANT * (std::sqrt(MATH_PI * 2 * var)))),
          mean + std::sqrt(-2 * var * std::log(FLOAT_TOLERANT * (std::sqrt(MATH_PI * 2 * var))))
          )
    {
      ASSERT(var > 0);
      const Float two_var = 2 * var;
      const Float coef = 1 / std::sqrt(MATH_PI * two_var);
      Float x = from;
      for (int i = 0; i < n_samples; ++i) {
        pdf[i] = coef * std::exp(-(x - mean) * (x - mean) / two_var);
        x += dx;
      }
      init_cdf();
    }

    point2f sample_disk(const point2f& u) {
      point2f u_offset = 2 * u - point2f(1, 1);
      if (u_offset.is_zero()) return { 0, 0 };
      Float theta, r;
      if (std::abs(u_offset.x) > std::abs(u_offset.y)) {
        r = u_offset.x;
        theta = PI_OVER_FOUR * (u_offset.y / u_offset.x);
      } else {
        r = u_offset.y;
        theta = PI_OVER_TWO - PI_OVER_FOUR * (u_offset.x / u_offset.y);
      }
      return r * point2f(std::cos(theta), std::sin(theta));
    }

    point2f sample_disk_distorted(const point2f& u) {
      Float r = std::sqrt(u.x);
      return r * point2f(TWO_PI * u.y);
    }

    point3f sample_hemisphere(const point2f& u) {
      Float z = u.x;
      Float r = std::sqrt(1 - z * z);
      Float two_pi_u = TWO_PI * u.y;
      return right_to_left({ std::cos(two_pi_u) * r, std::sin(two_pi_u) * r, z });
    }

    point3f sample_cosine_hemisphere(const point2f& u) {
      point2f proj = sample_disk(u);
      return right_to_left({ proj.x, proj.y, std::sqrt(std::max(Float(0), 1 - dot2(proj))) });
    }

    point3f sample_sphere(const point2f& u) {
      Float z = 1 - 2 * u.x;
      Float r = 2 * std::sqrt(u.x * (1 - u.x));
      Float two_pi_u = TWO_PI * u.y;
      return right_to_left({ std::cos(two_pi_u) * r, std::sin(two_pi_u) * r, z });
    }

    point3f sample_henyey_greenstein(Float g, const point2f& u) {
      if (COMPARE_EQ(g, 0)) return sample_sphere(u); // g = 0 gives isotropic scattering
      Float frac = (1 - pow2(g)) / (1 - g + 2 * g * u.x);
      Float cos_theta = (1 + pow2(g) - pow2(frac)) / (2 * g);
      Float phi = TWO_PI * u.y;
      Float sin_theta = sin_from_cos(cos_theta);
      return right_to_left({ sin_theta * std::cos(phi), sin_theta * std::sin(phi), cos_theta });
    }

    void sample_orthogonals(const vector3f& n, vector3f* u, vector3f* v, random::rng& rng) {
      vector3f tmp;
      do {
        // rejection sampling ;)
        tmp = rng.next_3uf();
      } while (COMPARE_EQ(pow2(tmp.dot(n)), tmp.size_sq()));
      *u = n.cross(tmp).normalized();
      *v = u->cross(n).normalized();
    }

    void sample_stratified_2d(
        std::vector<point2f>& samples,
        size_t n_samples,
        const point2i& n_strata,
        random::rng& rng)
    {
      ASSERT(n_strata.x > 0 && n_strata.y > 0);
      samples.resize(n_samples);
      const point2f stratum_size(Float(1) / n_strata.x, Float(1) / n_strata.y);
      size_t i = 0;
      while (i < n_samples) {
        for (int x = 0; x < n_strata.x; ++x) {
          for (int y = 0; y < n_strata.y; ++y) {
            const point2f u = rng.next_2uf();
            samples[i] = (point2f((x + u.x) * stratum_size.x, (y + u.y) * stratum_size.y)
              .clamped(FLOAT_TOLERANT, ONE_MINUS_FLOAT_TOLERANT));
            if (++i >= n_samples) return;
          }
        }
      }
    }

    Float sample_finite_norm_logistic(Float s, Float u, Float a, Float b) {
      Float integral = logistic_cdf(s, b) - logistic_cdf(s, a);
      Float raw = -s * std::log(1.f / (u * integral + logistic_cdf(s, a)) - 1.f);
      if (std::isnan(raw)) return 0;
      return math::clamp(raw, a, b);
    }
  } /* namespace sampler */
} /* namespace math */
