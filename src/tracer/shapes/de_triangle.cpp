#include "tracer/shapes/de_triangle.hpp"
#include "math/util.hpp"

namespace tracer {
  namespace shapes {
    de_triangle::de_triangle(
        const tf::transform& shape_to_world,
        const std::shared_ptr<material>& surface,
        const point3f& a,
        const point3f& b,
        const point3f& c)
      : destimator(shape_to_world, surface), a(a), b(b), c(c),
      ba(b-a), cb(c-b), ac(a-c), normal(ba.cross(ac).normalized()) {}

    normal3f de_triangle::calculate_normal(
        const point3f& p,
        Float delta,
        const ray& r,
        const normal3f& default_normal
        ) const
    {
      const vector3f n = (r.dir.dot(normal) < 0) ? normal : normal3f(-normal);
      return n.is_zero() ? default_normal : n;
    }

    Float de_triangle::distance_function(const point3f& p) const {
      const vector3f pa = p - a;
      const vector3f pb = p - b;
      const vector3f pc = p - c;
      return sqrt(
          (sign(ba.cross(normal).dot(pa)) +
           sign(cb.cross(normal).dot(pb)) +
           sign(ac.cross(normal).dot(pc)) < 2.0)
          ?
          std::min(std::min(
            dot2(ba*clamp(ba.dot(pa)/dot2(ba),Float(0),Float(1))-pa),
            dot2(cb*clamp(cb.dot(pb)/dot2(cb),Float(0),Float(1))-pb)),
            dot2(ac*clamp(ac.dot(pc)/dot2(ac),Float(0),Float(1))-pc))
          :
          normal.dot(pa)*normal.dot(pa)/dot2(normal) );
    }
        
    bounds3f de_triangle::bounds() const {
      return bounds3f(a).merge(bounds3f(b)).merge(bounds3f(c));
    }
  }
}
