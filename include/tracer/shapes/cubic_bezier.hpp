#ifndef TRACER_SHAPES_CUBIC_BEZIER_HPP
#define TRACER_SHAPES_CUBIC_BEZIER_HPP

#include "tracer/shape.hpp"

namespace tracer {
  namespace shapes {
    class cubic_bezier : public shape {
      private:
        point3f control_points[4];
        const Float thickness;

      public:
        enum cps_position {
          HEAD, BODY, TAIL
        };

        cubic_bezier(
            const tf::transform& shape_to_world,
            const std::shared_ptr<material>& surface,
            const point3f catmullrom_cps[4],
            Float thickness
            );

        cubic_bezier(
            const tf::transform& shape_to_world,
            const std::shared_ptr<material>& surface,
            const point3f catmullrom_cps[4],
            Float thickness,
            cps_position position
            );

        bounds3f bounds() const override;

        bool intersect_shape(
            const ray& r,
            const intersect_opts& options,
            intersect_result* result)
          const override;
    };
  }
}

#endif /* TRACER_SHAPES_CUBIC_BEZIER_HPP */
