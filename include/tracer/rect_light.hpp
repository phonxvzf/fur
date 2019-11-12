#ifndef LIGHT_SOURCE_HPP
#define LIGHT_SOURCE_HPP

#include "light_source.hpp"

namespace tracer {

  class rect_light : public light_source {
    private:
      const rgb_spectrum color;
      const point2f p_min;
      const point2f p_max;
      const Float inv_area;

    public:
      rect_light(
          const tf::transform& tf_local_to_world,
          const rgb_spectrum& color,
          const point2f& p_min,
          const point2f& p_max,
          size_t spp)
        : light_source(tf_local_to_world, spp),
        color(color),
        p_min(p_min),
        p_max(p_max),
        inv_area(1 / ((p_max.x - p_min.x) * (p_max.y - p_min.y))) {}

      emitter sample(const point2f& u) const override;
      Float pdf() const override;
  };

}

#endif /* LIGHT_SOURCE_HPP */
