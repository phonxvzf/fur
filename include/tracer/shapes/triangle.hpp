#ifndef TRACER_SHAPES_TRIANGLE_HPP
#define TRACER_SHAPES_TRIANGLE_HPP

#include "tracer/shape.hpp"

namespace tracer {
  namespace shapes {
    class triangle : public shape {
      private:
        const point3f a, b, c;
        const vector3f ab, bc, ca;
        const normal3f normal;

      public:
        triangle(
            const tf::transform& shape_to_world,
            const std::shared_ptr<material>& surface,
            const point3f& a,
            const point3f& b,
            const point3f& c,
            const normal3f& normal = 0
            );

        bounds3f bounds() const override;
        bounds3f world_bounds_explicit() const override;

        bool intersect_shape(
            const ray& r,
            const intersect_opts& options,
            intersect_result* result)
          const override;
    };
  }
}

#endif /* TRACER_SHAPES_TRIANGLE_HPP */
