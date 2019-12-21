#include <iostream>

#include "tracer/shape.hpp"
#include "math/util.hpp"

namespace tracer {

  shape::shape(
      const tf::transform& shape_to_world,
      const std::shared_ptr<material>& surface
      )
    : surface(surface),
    tf_shape_to_world(shape_to_world),
    tf_world_to_shape(shape_to_world.inverse()) {}

  shape::shape(const shape& cpy)
    : surface(cpy.surface),
    tf_shape_to_world(cpy.tf_shape_to_world),
    tf_world_to_shape(cpy.tf_world_to_shape) {}

  bounds3f shape::world_bounds() const {
    // TODO: cache this
    return tf_shape_to_world(bounds());
  }

  bool shape::intersect(
      const ray& r,
      const intersect_opts& options,
      intersect_result* result) const
  {
    const ray sray(tf_world_to_shape(r).normalized());
    const bool hit = intersect_shape(sray, options, result);
    if (hit) {
      const vector3f td = result->hit_point - r.origin;
      if (!COMPARE_EQ(r.dir.x, 0)) result->t_hit = td.x * r.inv_dir.x;
      if (!COMPARE_EQ(r.dir.y, 0)) result->t_hit = td.y * r.inv_dir.y;
      if (!COMPARE_EQ(r.dir.z, 0)) result->t_hit = td.z * r.inv_dir.z;
    }
    return hit;
  }

  destimator::destimator(
      const tf::transform& shape_to_world,
      const std::shared_ptr<material>& surface
      )
    : shape(shape_to_world, surface) {}

  normal3f destimator::calculate_normal(
          const point3f& p,
          Float delta,
          const normal3f& default_normal
          ) const
  {
    // compute the gradient using center finite differences
    const vector3f vx(delta, 0.0, 0.0);
    const vector3f vy(0.0, delta, 0.0);
    const vector3f vz(0.0, 0.0, delta);
    const Float inv_td = 1 / (2 * delta);
    const Float df_by_dx = (distance_function(p + vx) - distance_function(p - vx)) * inv_td;
    const Float df_by_dy = (distance_function(p + vy) - distance_function(p - vy)) * inv_td;
    const Float df_by_dz = (distance_function(p + vz) - distance_function(p - vz)) * inv_td;
    normal3f normal(df_by_dx, df_by_dy, df_by_dz);
    return normal.is_zero() ? default_normal : normal3f(normal.normalized());
  }

  normal3f destimator::calculate_normal(
          const point3f& p,
          Float delta,
          const ray& r,
          const normal3f& default_normal
          ) const
  {
    return calculate_normal(p, delta, default_normal);
  }

  bool destimator::intersect_shape(
      const ray& r,
      const intersect_opts& options,
      intersect_result* result
      ) const
  {
    Float t = 0;
    for (int i = 0; i < options.trace_max_iters; ++i) {
      const point3f phit = r(t);
      const Float dist = distance_function(phit);
      if (dist < options.hit_epsilon) {
        if (result != nullptr) {
          result->t_hit = t;
          result->hit_point = tf_shape_to_world(phit);
          result->normal = tf_shape_to_world(
              calculate_normal(phit, options.normal_delta, r, normal3f(0, 1, 0))
              ).normalized();
          result->object = this;
        }
        return true;
      }

      t += dist;
      if (t >= r.t_max) {
        break;
      }
    }

    return false;
  }

} /* namespace tracer */
