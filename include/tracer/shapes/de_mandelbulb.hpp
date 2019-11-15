#ifndef TRACER_SHAPES_DE_MANDELBULB_HPP
#define TRACER_SHAPES_DE_MANDELBULB_HPP

#include "tracer/shape.hpp"

namespace tracer {
  namespace shapes {
    class de_mandelbulb : public destimator {
      private:
        Float distance_function(const point3f& p) const override;

      public:
        de_mandelbulb(
            const tf::transform& shape_to_world,
            const std::shared_ptr<material>& surface);
        de_mandelbulb(const de_mandelbulb& cpy);
    };
  }
}

#endif /* TRACER_SHAPES_DE_MANDELBULB_HPP */
