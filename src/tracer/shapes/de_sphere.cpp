#include "tracer/shapes/de_sphere.hpp"

namespace tracer {
  namespace shapes {
    de_sphere::de_sphere(const matrix4f& shape_to_world, Float radius)
      : destimator(shape_to_world), radius(radius) {}

    de_sphere::de_sphere(const de_sphere& cpy)
      : destimator(cpy.tf_shape_to_world), radius(cpy.radius) {}

    vector3f de_sphere::calculate_normal(
        const point3f& p,
        Float delta,
        const vector3f& default_normal
        ) const
    {
      return 2 * p;
    }

    Float de_sphere::distance_function(const point3f& p) const {
      return std::sqrt(p.dot(p)) - radius;
    }
  }
}
