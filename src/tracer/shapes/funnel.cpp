#include "tracer/shapes/funnel.hpp"
#include "math/util.hpp"

namespace tracer {
  namespace shapes {
    funnel::funnel(
        const tf::transform& shape_to_world,
        const std::shared_ptr<material>& surface,
        Float radius,
        Float height)
      : shape(shape_to_world, surface), radius(radius), height(height) {}

    bounds3f funnel::bounds() const {
      return bounds3f({ -radius, 0, -radius }, { radius, height, radius });
    }

    bool funnel::intersect_shape(
        const ray& r,
        const intersect_opts& options,
        intersect_result* result) const
    {
      const Float r_over_h_sq = pow2(radius / height);
      const Float a = pow2(r.dir.x) + pow2(r.dir.z) - r_over_h_sq * pow2(r.dir.y);
      const Float b = 2 * (r_over_h_sq * (height * r.dir.y - r.origin.y * r.dir.y)
          + r.origin.x * r.dir.x + r.origin.z * r.dir.z);
      const Float c = pow2(r.origin.x)
        - r_over_h_sq * (pow2(r.origin.y) - 2 * height * r.origin.y + pow2(height))
        + pow2(r.origin.z);

      Float t_min, t_max;
      const Float delta = solve_quadratic(&t_min, &t_max, a, b, c);

      if (delta < 0) return false;
      if (t_min > t_max) std::swap(t_min, t_max);

      /*
      point3f hit_point = r(t_min);
      Float t = r.medium == INSIDE ? t_max : t_min;
      Float dont_flip_normal = 1;
      if (hit_point.y < 0) {
        hit_point = r(t_max);
        t = t_max;
        if (r.medium == OUTSIDE) dont_flip_normal = -1.f;
      }
      */

      Float t = r.medium == INSIDE ? t_max : t_min;
      point3f hit_point = r(t);
      Float dont_flip_normal = 1;

      if (t < 0) return false;
      if (hit_point.y < 0 || hit_point.y > height) return false;

      const normal3f grad {
        hit_point.x, r_over_h_sq * (hit_point.y - height), hit_point.z
      };

      if (result != nullptr) {
        result->t_hit = t;
        result->hit_point = tf_shape_to_world(hit_point);
        result->normal = dont_flip_normal * tf_shape_to_world(grad).normalized();
        result->object = this;
      }

      return true;
    }
  }
}
