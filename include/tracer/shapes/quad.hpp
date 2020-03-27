#ifndef TRACER_SHAPES_QUAD_HPP
#define TRACER_SHAPES_QUAD_HPP

#include "tracer/shape.hpp"

namespace tracer {
  namespace shapes {
    class quad : public shape {
      private:
        const point3f a, b, c, d;
        const vector3f ab, bc, cd, da;
        const normal3f normal;

      public:
        quad(
            const tf::transform& shape_to_world,
            const std::shared_ptr<material>& surface,
            const point3f& a,
            const point3f& b,
            const point3f& c,
            const point3f& d
            );

        bounds3f bounds() const override;

        bool intersect_shape(
            const ray& r,
            const intersect_opts& options,
            intersect_result* result)
          const override;

        point3f sample(const point2f& u) const override;
        Float pdf() const override;
    };
  }
}

#endif /* TRACER_SHAPES_QUAD_HPP */
