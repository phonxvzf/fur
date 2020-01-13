#include "tracer/shapes/de_box.hpp"
#include "math/util.hpp"

namespace tracer {
  namespace shapes {
    de_box::de_box(
        const tf::transform& shape_to_world,
        const std::shared_ptr<material>& surface,
        const point3f b)
      : destimator(shape_to_world, surface), b(b) {}

    de_box::de_box(const de_box& cpy) : destimator(cpy), b(cpy.b) {}

    Float de_box::distance_function(const point3f& p) const {
      const vector3f q = point3f(std::abs(p.x), std::abs(p.y), std::abs(p.z)) - b;
      const Float qx = std::max(q.x, Float(0));
      const Float qy = std::max(q.y, Float(0));
      const Float qz = std::max(q.z, Float(0));
      return vector3f(qx, qy, qz).size() + std::min(std::max(qx, std::max(qy, qz)), Float(0));
    }

    bounds3f de_box::bounds() const {
      return bounds3f(-b, b);
    }
  }
}
