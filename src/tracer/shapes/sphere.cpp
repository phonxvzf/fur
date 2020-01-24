#include "tracer/shapes/sphere.hpp"
#include "math/util.hpp"

namespace tracer {
  namespace shapes {
    sphere::sphere(
        const tf::transform& shape_to_world,
        const std::shared_ptr<material>& surface,
        Float radius)
      : shape(shape_to_world, surface), radius(radius) {}

    bounds3f sphere::bounds() const {
      return bounds3f({ radius, radius, radius }, { radius, radius, radius });
    }

    bool sphere::intersect_shape(
        const ray& r,
        const intersect_opts& options,
        intersect_result* result) const
    {
      Float a = dot2(r.dir);
      Float b = 2 * r.origin.dot(r.dir);
      Float c = dot2(r.origin) - pow2(radius);
      Float delta = pow2(b) - 4 * a * c;
      if (delta < 0) return false;

      Float sqrt_delta = std::sqrt(delta);
      Float t = (-b + ((r.medium == INSIDE) ? sqrt_delta : -sqrt_delta)) / (2 * a);

      if (t < 0) return false;

      point3f hit_point = r(t);
      if (result != nullptr) {
        result->t_hit = t;
        result->hit_point = tf_shape_to_world(hit_point);
        result->normal = tf_shape_to_world(normal3f(hit_point)).normalized();
        result->object = this;
      }

      return true;
    }
  }
}
