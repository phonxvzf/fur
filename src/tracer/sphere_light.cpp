#include "tracer/sphere_light.hpp"
#include "math/sampler.hpp"

namespace tracer {

  light_source::emitter sphere_light::sample(const point2f& u) const {
    point3f pt = radius * sampler::sample_sphere(u);
    return light_source::emitter(
        tf_local_to_world(pt),
        color,
        tf_local_to_world(normal3f(pt.normalized())),
        this
        );
  }

  Float sphere_light::pdf() const {
    return inv_area;
  }

}
