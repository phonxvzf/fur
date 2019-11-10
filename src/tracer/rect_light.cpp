#include "tracer/rect_light.hpp"

namespace tracer {
  light_source::emitter rect_light::sample_light(const point2f& u) const {
    const vector2f size = p_max - p_min;
    return light_source::emitter(
        tf_local_to_world(point3f(p_min.x + size.x * u.x, p_min.y + size.y * u.y)),
        color
        );
  }
}
