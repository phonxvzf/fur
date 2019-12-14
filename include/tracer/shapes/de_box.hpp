#ifndef TRACER_SHAPES_DE_BOX_HPP
#define TRACER_SHAPES_DE_BOX_HPP

#include "tracer/shape.hpp"

namespace tracer {
  namespace shapes {
    class de_box : public destimator {
      private:
        const point3f b;

        Float distance_function(const point3f& p) const override;

      public:
        de_box(
            const tf::transform& shape_to_world,
            const std::shared_ptr<material>& surface,
            const point3f b);

        de_box(const de_box& cpy);

        bounds3f bounds() const override;
    };
  }
}

#endif /* TRACER_SHAPES_DE_BOX_HPP */
