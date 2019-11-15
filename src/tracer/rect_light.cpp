#include "tracer/rect_light.hpp"

namespace tracer {

  light_source::emitter rect_light::sample(const point2f& u) const {
    const vector2f size = p_max - p_min;
    return light_source::emitter(
        tf_local_to_world(point3f(p_min.x + size.x * u.x, p_min.y + size.y * u.y, 0)),
        color,
        tf_local_to_world(normal3f(0, 0, 1)).normalized(),
        this
        );
  }
      
  Float rect_light::pdf() const {
    return inv_area;
  }
}
