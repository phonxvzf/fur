#include "tracer/point_light.hpp"

namespace tracer {
  light_source::emitter point_light::sample_light(const point2f& u) const {
    return light_source::emitter(tf_local_to_world(point3f(0.0f)), color);
  }
}
