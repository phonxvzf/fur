#include "tracer/rect_light.hpp"

namespace tracer {

  light_source::emitter rect_light::sample(const point2f& u) const {
    const vector2f size = p_max - p_min;
    const vector3f a(p_max.x - p_min.x, p_min.y, 0);
    const vector3f b(p_max.x, p_max.y - p_min.y, 0);
    return light_source::emitter(
        tf_local_to_world(point3f(p_min.x + size.x * u.x, p_min.y + size.y * u.y)),
        color,
        tf_local_to_world(normal3f(a.cross(b).normalized())),
        this
        );
  }
      
  Float rect_light::pdf() const {
    return inv_area;
  }
}
