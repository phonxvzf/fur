#include "tracer/shapes/de_sphere.hpp"
#include "math/util.hpp"

namespace tracer {
  namespace shapes {
    de_sphere::de_sphere(
        const tf::transform& shape_to_world,
        const std::shared_ptr<material>& surface,
        Float radius)
      : destimator(shape_to_world, surface), radius(radius) {}

    de_sphere::de_sphere(const de_sphere& cpy)
      : destimator(cpy.tf_shape_to_world, cpy.surface), radius(cpy.radius) {}

    normal3f de_sphere::calculate_normal(
        const point3f& p,
        Float delta,
        const normal3f& default_normal
        ) const
    {
      return normal3f(p);
    }

    Float de_sphere::distance_function(const point3f& p) const {
      return std::sqrt(p.dot(p)) - radius;
    }

    bounds3f de_sphere::bounds() const {
      return bounds3f(-point3f(SQRT_TWO * radius), point3f(SQRT_TWO * radius));
    }
  }
}
