#include "tracer/light_source.hpp"
#include "math/util.hpp"

namespace tracer {

  Float light_source::pdf(const point3f& hit_point, const emitter& emt) const {
    const vector3f omega = hit_point - emt.position;
    const Float r2 = omega.size_sq();
    const Float dot = std::max(Float(0), omega.normalized().dot(emt.normal));
    if (COMPARE_EQ(dot, 0)) return 0;
    return pdf() * r2 / dot;
  }

}
