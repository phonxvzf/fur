#include "tracer/light_source.hpp"
#include "math/util.hpp"

namespace tracer {

  Float light_source::pdf(const point3f& hit_point, const emitter& emt) const {
    const vector3f omega = hit_point - emt.position;
    const Float r2 = omega.size_sq();
    const normal3f normal = emt.normal.is_zero() ? normal3f(omega.normalized()) : emt.normal;
    const Float dot = std::max(Float(0), omega.normalized().dot(normal));
    if (COMPARE_EQ(dot, 0)) return 0;
    return pdf() * r2 / dot;
  }

}
