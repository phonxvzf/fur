#include "math/sampler.hpp"
#include "math/util.hpp"
#include "error.hpp"

namespace math {
  namespace sampler {
    uniform1d::uniform1d(int n_samples, Float a, Float b)
      : pdf1d(n_samples, a, b)
    {
      ASSERT(!COMPARE_EQ(a, b));
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
  }
}
