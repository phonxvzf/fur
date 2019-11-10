#ifndef TRACER_LIGHT_SOURCE_HPP
#define TRACER_LIGHT_SOURCE_HPP

#include <memory>

#include "math/transform.hpp"
#include "tracer/spectrum.hpp"

namespace tracer {

  using namespace math;

  class light_source {
    protected:
      const tf::transform tf_local_to_world;

    public:
      struct emitter {
        const point3f position;
        const nspectrum color;

        emitter(const point3f& position, const nspectrum& color)
          : position(position), color(color) {}
      };

      light_source(const tf::transform& tf_local_to_world)
        : tf_local_to_world(tf_local_to_world) {}

      light_source(const light_source& cpy) : tf_local_to_world(cpy.tf_local_to_world) {}

      virtual emitter sample_light(const point2f& u) const = 0;
  };
} /* namespace tracer */

#endif /* TRACER_LIGHT_SOURCE_HPP */
