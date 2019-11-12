#ifndef SPHERE_LIGHT_HPP
#define SPHERE_LIGHT_HPP

#include "light_source.hpp"
#include "math/util.hpp"

namespace tracer {

  class sphere_light : public light_source {
    private:
      const rgb_spectrum color;
      const Float radius;
      const Float inv_area;

    public:
      sphere_light(
          const tf::transform& tf_local_to_world,
          const rgb_spectrum& color,
          Float radius,
          size_t spp)
        : light_source(tf_local_to_world, spp),
        color(color),
        radius(radius),
        inv_area(1 / (FOUR_PI * radius * radius)) {}

      emitter sample(const point2f& u) const override;
      Float pdf() const override;
  };

}

#endif /* SPHERE_LIGHT_HPP */
