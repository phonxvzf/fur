#ifndef TRACER_SHAPES_DE_MANDELBULB_HPP
#define TRACER_SHAPES_DE_MANDELBULB_HPP

#include "tracer/shape.hpp"

namespace tracer {
  namespace shapes {
    class de_mandelbulb : public destimator {
      private:
        Float distance_function(const point3f& p) const;

      public:
        de_mandelbulb(const matrix4f& shape_to_world);
        de_mandelbulb(const de_mandelbulb& cpy);
    };
  }
}

#endif /* TRACER_SHAPES_DE_MANDELBULB_HPP */
