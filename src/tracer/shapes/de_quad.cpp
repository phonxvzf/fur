#include "tracer/shapes/de_quad.hpp"
#include "math/util.hpp"

namespace tracer {
  namespace shapes {
    de_quad::de_quad(
        const tf::transform& shape_to_world,
        const std::shared_ptr<material>& surface,
        const point3f& a,
        const point3f& b,
        const point3f& c,
        const point3f& d
        )
      : destimator(shape_to_world, surface), a(a), b(b), c(c), d(d),
      ba(b-a), cb(c-b), dc(d-c), ad(a-d), normal(ba.cross(ad).normalized()) {}
        
    de_quad::de_quad(const de_quad& cpy)
      : destimator(cpy.tf_shape_to_world, cpy.surface),
      a(cpy.a), b(cpy.b), c(cpy.c), d(cpy.d), ba(cpy.ba), cb(cpy.cb), dc(cpy.dc), ad(cpy.ad) {}

    normal3f de_quad::calculate_normal(
        const point3f& p,
        Float delta,
        const ray& r,
        const normal3f& default_normal
        ) const
    {
      const vector3f n = (r.dir.dot(normal) < 0) ? normal : normal3f(-normal);
      return n.is_zero() ? default_normal : n;
    }

    Float de_quad::distance_function(const point3f& p) const {
      const vector3f pa = p - a;
      const vector3f pb = p - b;
      const vector3f pc = p - c;
      const vector3f pd = p - d;

      return sqrt(
          (sign(ba.cross(normal).dot(pa)) +
           sign(cb.cross(normal).dot(pb)) +
           sign(dc.cross(normal).dot(pc)) +
           sign(ad.cross(normal).dot(pd)) < 3.0)
          ?
          std::min(std::min(std::min(
            dot2(ba*clamp(ba.dot(pa)/dot2(ba),Float(0),Float(1))-pa),
            dot2(cb*clamp(cb.dot(pb)/dot2(cb),Float(0),Float(1))-pb)),
            dot2(dc*clamp(dc.dot(pc)/dot2(dc),Float(0),Float(1))-pc)),
            dot2(ad*clamp(ad.dot(pd)/dot2(ad),Float(0),Float(1))-pd))
          :
          normal.dot(pa)*normal.dot(pa)/dot2(normal));
    }

    bounds3f de_quad::bounds() const {
      return bounds3f(a).merge(bounds3f(b)).merge(bounds3f(c)).merge(bounds3f(d));
    }
  }
}
