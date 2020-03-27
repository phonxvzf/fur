#include "tracer/shapes/quad.hpp"
#include "math/sampler.hpp"

#warning quad sampling only supports rectangular geometry

namespace tracer {
  namespace shapes {
    quad::quad(
        const tf::transform& shape_to_world,
        const std::shared_ptr<material>& surface,
        const point3f& a,
        const point3f& b,
        const point3f& c,
        const point3f& d
        )
      : shape(shape_to_world, surface),
      a(a), b(b), c(c), d(d), ab(b-a), bc(c-b), cd(d-c), da(a-d),
      normal((b-a).cross(a-d).normalized()) {}

    bounds3f quad::bounds() const {
      return bounds3f(a).merge(bounds3f(b)).merge(bounds3f(c)).merge(bounds3f(d));
    }

    bool quad::intersect_shape(
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
      vector3f dp = p - d;

      if (ab.cross(ap).dot(normal) > 0 ||
          bc.cross(bp).dot(normal) > 0 ||
          cd.cross(cp).dot(normal) > 0 ||
          da.cross(dp).dot(normal) > 0)
        return false;

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

    point3f quad::sample(const point2f& u) const {
      Float w = b.x - d.x;
      Float h = b.z - d.z;
      return tf_shape_to_world(point3f(c.x + u[0] * w, 0, c.z + u[1] * h));
    }

    Float quad::pdf() const {
      Float w = b.x - d.x;
      Float h = b.z - d.z;
      return 1.f / (w * h);
    }
  }
}
