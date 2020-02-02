#include "tracer/shapes/disk.hpp"

namespace tracer {
  namespace shapes {
    disk::disk(
        const tf::transform& shape_to_world,
        const std::shared_ptr<material>& surface,
        Float radius
        )
      : shape(shape_to_world, surface), radius(radius), radius2(radius * radius) {}

    bounds3f disk::bounds() const {
      return { { -radius, 0, -radius }, { radius, 0, radius } };
    }

    bool disk::intersect_shape(
        const ray& r,
        const intersect_opts& options,
        intersect_result* result) const
    {
      normal3f normal(0, 1, 0);
      if (COMPARE_EQ(r.dir.dot(normal), 0)) return false; // ray is parallel to the plane

      Float t = (-r.origin).dot(normal) / r.dir.dot(normal);
      if (t < 0) return false;

      const point3f p = r(t);
      if (p.dot(p) > radius2) return false;

      if (result != nullptr) {
        result->hit_point = tf_shape_to_world(p);
        result->t_hit = t;
        result->normal = tf_shape_to_world((r.dir.dot(normal) < 0) ? normal : normal3f(-normal))
          .normalized();
        result->object = this;
        if (r.medium == INSIDE) result->normal = -result->normal;
      }

      return true;
    }
  }
}
