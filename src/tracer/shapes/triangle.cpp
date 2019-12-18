#include "tracer/shapes/triangle.hpp"

namespace tracer {
  namespace shapes {
    triangle::triangle(
        const tf::transform& shape_to_world,
        const std::shared_ptr<material>& surface,
        const point3f& a,
        const point3f& b,
        const point3f& c
        )
      : shape(shape_to_world, surface),
      a(a), b(b), c(c), ab(b-a), bc(c-b), ca(a-c), normal((b-a).cross(a-c).normalized()) {}

    bounds3f triangle::bounds() const {
      return bounds3f(a).merge(bounds3f(b)).merge(bounds3f(c));
    }

    bool triangle::intersect_shape(
        const ray& r,
        const intersect_opts& options,
        intersect_result* result) const
    {
      if (COMPARE_EQ(r.dir.dot(normal), 0)) return false; // ray is parallel to the plane

      Float t = (a - r.origin).dot(normal) / r.dir.dot(normal);
      if (t < 0) return false;

      point3f p = r(t);
      vector3f ap = p - a;
      vector3f bp = p - b;
      vector3f cp = p - c;

      if ((ab.cross(ap).dot(normal) > 0)
          || (bc.cross(bp).dot(normal) > 0)
          || (ca.cross(cp).dot(normal) > 0)) return false;

      if (result != nullptr) {
        result->hit_point = tf_shape_to_world(p);
        result->t_hit = t;
        result->normal = tf_shape_to_world((r.dir.dot(normal) < 0) ? normal : normal3f(-normal))
          .normalized();
        result->object = this;
      }

      return true;
    }
  }
}
