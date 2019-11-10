#include "light_source.hpp"

namespace tracer {

  class rect_light : public light_source {
    private:
      const rgb_spectrum color;
      const point2f& p_min;
      const point2f& p_max;

    public:
      rect_light(
          const tf::transform& tf_local_to_world,
          const rgb_spectrum& color,
          const point2f& p_min,
          const point2f& p_max)
        : light_source(tf_local_to_world), color(color), p_min(p_min), p_max(p_max) {}

      emitter sample_light(const point2f& u) const override;
  };

}
