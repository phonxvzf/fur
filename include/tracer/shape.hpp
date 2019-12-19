#ifndef TRACER_SHAPE_HPP
#define TRACER_SHAPE_HPP

#include <memory>

#include "math/transform.hpp"
#include "tracer/ray.hpp"
#include "tracer/material.hpp"

namespace tracer {
  class shape {
    public:
      struct intersect_opts {
        Float hit_epsilon     = 1e-4;
        Float bias_epsilon    = 1e-4;
        Float normal_delta    = 1e-4;
        int trace_max_iters   = 1000;
      };

      struct intersect_result {
        Float     t_hit = std::numeric_limits<Float>::max();
        point3f   hit_point;
        normal3f  normal;
        const shape* object = nullptr;
      };

      const std::shared_ptr<material> surface;

      shape(
          const tf::transform& shape_to_world,
          const std::shared_ptr<material>& surface
          );
      shape(const shape& cpy);

      virtual bounds3f bounds() const = 0;
      bounds3f world_bounds() const;

      virtual bool intersect(
          const ray& r,
          const intersect_opts& options,
          intersect_result* result
          ) const;

    protected:
      const tf::transform tf_shape_to_world;
      const tf::transform tf_world_to_shape;

      virtual bool intersect_shape(
          const ray& r,
          const intersect_opts& options,
          intersect_result* result
          ) const = 0;
  };

  class destimator : public shape {
    protected:
      virtual normal3f calculate_normal(
          const point3f& p,
          Float delta,
          const normal3f& default_normal
          ) const;

      virtual normal3f calculate_normal(
          const point3f& p,
          Float delta,
          const ray& r,
          const normal3f& default_normal
          ) const;

      virtual Float distance_function(const point3f& p) const = 0;

      bool intersect_shape(
          const ray& r,
          const intersect_opts& options,
          intersect_result* result)
        const override;

    public:
      destimator(
          const tf::transform& shape_to_world,
          const std::shared_ptr<material>& surface
          );
  };
}

#endif /* TRACER_SHAPE_HPP */
