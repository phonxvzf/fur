#ifndef MATH_SAMPLER_HPP
#define MATH_SAMPLER_HPP

#include "pdf.hpp"
#include "vector.hpp"

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

    point2f sample_disk(const point2f& u);
    point2f sample_disk_distorted(const point2f& u);
    point3f sample_hemisphere(const point2f& u);
    point3f sample_cosine_hemisphere(const point2f& u);
    point3f sample_sphere(const point2f& u);
  }
}

#endif /* MATH_SAMPLER_HPP */
