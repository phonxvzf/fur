#ifndef TRACER_SHAPES_DE_TRIANGLE_HPP
#define TRACER_SHAPES_DE_TRIANGLE_HPP

#include "tracer/shape.hpp"

namespace tracer {
  namespace shapes {
    class de_triangle : public destimator {
      private:
        const point3f a;
        const point3f b;
        const point3f c;
        const point3f ba;
        const point3f cb;
        const point3f ac;
        const normal3f normal;

        normal3f calculate_normal(
            const point3f& p,
            Float delta,
            const ray& r,
            const normal3f& default_normal
            ) const override;

        Float distance_function(const point3f& p) const override;

      public:
        de_triangle(
            const tf::transform& shape_to_world,
            const std::shared_ptr<material>& surface,
            const point3f& a,
            const point3f& b,
            const point3f& c);

        de_triangle(const de_triangle& cpy);

        bounds3f bounds() const override;
    };
  }
}

#endif /* TRACER_SHAPES_DE_TRIANGLE_HPP */
