#ifndef TRACER_SHAPES_INF_SHPERES_HPP
#define TRACER_SHAPES_INF_SHPERES_HPP

#include "tracer/shape.hpp"

namespace tracer {
  namespace shapes {
    class de_inf_spheres : public destimator {
      private:
        Float distance_function(const point3f& p) const override;
        Float sphere_position(Float x) const;

        const Float radius;
        const Float cell_size;

      public:
        de_inf_spheres(
            const tf::transform& shape_to_world,
            const std::shared_ptr<material>& surface,
            Float radius,
            Float cell_size);
        de_inf_spheres(const de_inf_spheres& cpy);
    
        bounds3f bounds() const override;
    };
  }
}

#endif /* TRACER_SHAPES_INF_SHPERES_HPP */
