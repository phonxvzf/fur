#ifndef TRACER_SHAPE_HPP
#define TRACER_SHAPE_HPP

#include <memory>

#include "math/transform.hpp"
#include "tracer/ray.hpp"

namespace tracer {

  class shape {
    protected:
      const tf::transform tf_shape_to_world;
      const tf::transform tf_world_to_shape;

    public:
      struct intersect_opts {
        Float hit_epsilon;
        Float normal_delta;
        int trace_max_iters;

        intersect_opts() : hit_epsilon(1e-4), normal_delta(1e-4), trace_max_iters(1000) {}
      };

      struct intersect_result {
        Float     t_hit;
        point3f   hit_point;
        normal3f  normal;
        const shape* object = nullptr;

        ray debug;
      };

      shape(const tf::transform& shape_to_world);
      shape(const shape& cpy);

      virtual bool intersect(
          const ray& r,
          const intersect_opts& options,
          intersect_result* result
          ) const;
  };

  class destimator : public shape {
    protected:
      virtual normal3f calculate_normal(
          const point3f& p,
          Float delta,
          const vector3f& default_normal
          ) const;
      virtual Float distance_function(const point3f& p) const = 0;

    public:
      destimator(const tf::transform& shape_to_world);

      bool intersect(const ray& r, const intersect_opts& options, intersect_result* result) const;
  };
}

#endif /* TRACER_SHAPE_HPP */
