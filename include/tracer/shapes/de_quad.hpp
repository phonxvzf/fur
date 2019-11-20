#ifndef TRACER_SHAPES_DE_QUAD_HPP
#define TRACER_SHAPES_DE_QUAD_HPP

#include "tracer/shape.hpp"

namespace tracer {
  namespace shapes {
    class de_quad : public destimator {
      private:
        const point3f a;
        const point3f b;
        const point3f c;
        const point3f d;
        const vector3f ba;
        const vector3f cb;
        const vector3f dc;
        const vector3f ad;
        const normal3f normal;

        normal3f calculate_normal(
            const point3f& p,
            Float delta,
            const ray& r,
            const normal3f& default_normal
            ) const override;

        Float distance_function(const point3f& p) const override;

      public:
        de_quad(
            const tf::transform& shape_to_world,
            const std::shared_ptr<material>& surface,
            const point3f& a,
            const point3f& b,
            const point3f& c,
            const point3f& d
            );
        de_quad(const de_quad& cpy);

        bounds3f bounds() const override;
    };
  }
}

#endif /* TRACER_SHAPES_DE_QUAD_HPP */
