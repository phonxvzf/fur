#ifndef MATH_PDF_HPP
#define MATH_PDF_HPP

#include <vector>

#include "float.hpp"

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

      Float sample(Float u, Float offset) const;
      Float sample(Float u) const;
  };
}

#endif /* MATH_PDF_HPP */
