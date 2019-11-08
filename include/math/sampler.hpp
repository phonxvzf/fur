#ifndef MATH_SAMPLER_HPP
#define MATH_SAMPLER_HPP

#include "pdf.hpp"

namespace math {
  namespace sampler {
    class uniform1d : public pdf1d {
      public:
        uniform1d(int n_samples, Float a, Float b);
    };

    class normal1d : public pdf1d {
      public:
        normal1d(int n_samples, Float mean, Float var);
    };
  }
}

#endif /* MATH_SAMPLER_HPP */
