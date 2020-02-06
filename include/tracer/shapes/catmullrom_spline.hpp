#ifndef TRACER_SHAPES_CATMULLROM_SPLINE_HPP
#define TRACER_SHAPES_CATMULLROM_SPLINE_HPP

#include "tracer/shape.hpp"

namespace tracer {
  namespace shapes {
    class catmullrom_spline : public shape {
      private:
      public:
        catmullrom_spline(
            const tf::transform& shape_to_world,
            const std::shared_ptr<material>& surface,
            const std::vector<point3f>& control_points
            );
    };
  }
}

#endif /* TRACER_SHAPES_CATMULLROM_SPLINE_HPP */
