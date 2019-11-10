#ifndef TRACER_POINT_LIGHT_HPP
#define TRACER_POINT_LIGHT_HPP

#include "tracer/light_source.hpp"

namespace tracer {

  class point_light : public light_source {
    private:
      const rgb_spectrum color;

    public:
      point_light(const tf::transform& tf_local_to_world, const rgb_spectrum& color)
        : light_source(tf_local_to_world), color(color) {}

      emitter sample_light(const point2f& u) const override;
  };

}

#endif /* TRACER_POINT_LIGHT_HPP */
