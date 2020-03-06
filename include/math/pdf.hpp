#ifndef MATH_PDF_HPP
#define MATH_PDF_HPP

#include <vector>

#include "float.hpp"
#include "util.hpp"

namespace math {
  class pdf1d {
    protected:
      std::vector<Float> pdf;
      std::vector<Float> cdf;
      std::vector<std::pair<Float,int>> cdf_sorted;
      const int n_samples;
      const Float from;
      const Float to;
      const Float dx;

      void init_cdf();

    public:
      pdf1d(int n_samples, Float from, Float to);

      virtual Float sample(Float u, Float offset) const;
      virtual Float sample(Float u) const;
  };

  inline Float pdf_uniform_disk() {
    return INV_PI;
  }

  inline Float pdf_uniform_hemisphere() {
    return INV_TWO_PI;
  }

  inline Float pdf_uniform_sphere() {
    return INV_FOUR_PI;
  }

  inline Float pdf_uniform_cosine_hemisphere(Float theta) {
    return std::cos(theta) * INV_PI;
  }

  inline Float pdf_uniform_cosine_hemisphere_cos(Float cos_theta) {
    return cos_theta * INV_PI;
  }

  inline Float logistic_pdf(Float s, Float x) {
    Float inv_s = 1.f / s;
    Float ex = std::exp(-std::abs(x) * inv_s);
    return ex / (pow2(1 + ex)) * inv_s;
  }

  inline Float logistic_cdf(Float s, Float x) {
    return 1.f / (1 + std::exp(-x / s));
  }

  inline Float logistic_pdf_finite_norm(Float s, Float x, Float a, Float b) {
    return logistic_pdf(s, x) / (logistic_cdf(s, b) - logistic_cdf(s, a));
  }
} /* namespace math */

#endif /* MATH_PDF_HPP */
