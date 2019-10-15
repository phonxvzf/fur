#ifndef TRACER_SHAPE_HPP
#define TRACER_SHAPE_HPP

#include "math/transform.hpp"
#include "tracer/ray.hpp"
#include "tracer/intersection.hpp"

namespace tracer {
  class shape {
    protected:
      const matrix4f tf_shape_to_world;
      const matrix4f tf_world_to_shape;

    public:
      shape(const matrix4f& shape_to_world);
      shape(const shape& cpy);
      
      virtual bool intersect(
          const ray& r,
          const intersect_opts& options,
          intersect_result* result
          );
  };

  class destimator : public shape {
    private:
      virtual vector3f calculate_normal(
          const point3f& p,
          Float delta,
          const vector3f& default_normal
          ) const;
      virtual Float distance_function(const point3f& p) const = 0;

    public:
      destimator(const matrix4f& shape_to_world);

      bool intersect(const ray& r, const intersect_opts& options, intersect_result* result);
  };
}

#endif /* TRACER_SHAPE_HPP */
