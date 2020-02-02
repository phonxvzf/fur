#ifndef TRACER_SHAPES_DISK_HPP
#define TRACER_SHAPES_DISK_HPP

#include "tracer/shape.hpp"

namespace tracer {
  namespace shapes {
    class disk : public shape {
      private:
        const Float radius;
        const Float radius2;

      public:
        disk(
            const tf::transform& shape_to_world,
            const std::shared_ptr<material>& surface,
            Float radius
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

#endif /* TRACER_SHAPES_DISK_HPP */
