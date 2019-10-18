#ifndef TRACER_SHAPES_DE_SPHERE_HPP
#define TRACER_SHAPES_DE_SPHERE_HPP

#include "tracer/shape.hpp"

namespace tracer {
  namespace shapes {
    class de_sphere : public destimator {
      private:
        normal3f calculate_normal(
            const point3f& p,
            Float delta,
            const vector3f& default_normal
            ) const;
        Float distance_function(const point3f& p) const;

        const Float radius;

      public:
        de_sphere(const tf::transform& shape_to_world, Float radius);
        de_sphere(const de_sphere& cpy);
    };
  }
}

#endif /* TRACER_SHAPES_DE_SPHERE_HPP */
