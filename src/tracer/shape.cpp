#include "tracer/shape.hpp"
#include <iostream>

namespace tracer {

  shape::shape(const tf::transform& shape_to_world)
    : tf_shape_to_world(shape_to_world), tf_world_to_shape(shape_to_world.inverse()) {}
  shape::shape(const shape& cpy)
    : tf_shape_to_world(cpy.tf_shape_to_world), tf_world_to_shape(cpy.tf_world_to_shape) {}
      
  bool shape::intersect(const ray& r, const intersect_opts& options, intersect_result* result) const
  {
    return false;
  }

  destimator::destimator(const tf::transform& shape_to_world) : shape(shape_to_world) {}

  normal3f destimator::calculate_normal(
          const point3f& p,
          Float delta,
          const vector3f& default_normal
          ) const
  {
    // compute the gradient using center finite differences
    const vector3f vx(delta, 0.0, 0.0);
    const vector3f vy(0.0, delta, 0.0);
    const vector3f vz(0.0, 0.0, delta);
    const Float two_delta = 2 * delta;
    const Float df_by_dx = (distance_function(p + vx) - distance_function(p - vx)) / two_delta;
    const Float df_by_dy = (distance_function(p + vy) - distance_function(p - vy)) / two_delta;
    const Float df_by_dz = (distance_function(p + vz) - distance_function(p - vz)) / two_delta;
    const vector3f normal(df_by_dx, df_by_dy, df_by_dz);
    return normal.is_zero() ? default_normal : normal;
  }


  bool destimator::intersect(
      const ray& r,
      const intersect_opts& options,
      intersect_result* result
      ) const
  {
    const ray sray(tf_world_to_shape(r).normalized());
    if (result) result->debug = sray;
    Float t = 0;
    for (int i = 0; i < options.trace_max_iters; ++i) {
      const point3f phit = sray(t);
      const Float dist = distance_function(phit);
      if (dist < options.hit_epsilon) {
        if (result != nullptr) {
          result->t_hit = t;
          result->hit_point = tf_shape_to_world(phit);
          result->normal = tf_shape_to_world(
              calculate_normal(phit, options.normal_delta, vector3f(0.0))
              ).normalized();
          result->object = this;
        }
        return true;
      }
      t += dist;

      if (t >= sray.t_max) {
        break;
      }
    }

    return false;
  }

} /* namespace tracer */
