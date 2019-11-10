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
      const size_t spp;

      struct emitter {
        const point3f position;
        const nspectrum color;
        const normal3f normal;
        const light_source* parent;

        emitter(
            const point3f& position,
            const nspectrum& color,
            const normal3f& normal,
            const light_source* parent = nullptr
            )
          : position(position), color(color), normal(normal), parent(parent) {}
      };

      light_source(const tf::transform& tf_local_to_world, size_t spp = 1)
        : tf_local_to_world(tf_local_to_world), spp(spp) {}

      light_source(const light_source& cpy)
        : tf_local_to_world(cpy.tf_local_to_world), spp(cpy.spp) {}

      virtual emitter sample(const point2f& u) const = 0;
      virtual Float pdf() const = 0;
      Float pdf(const point3f& hit_point, const emitter& emt) const;
  };
} /* namespace tracer */

#endif /* TRACER_LIGHT_SOURCE_HPP */
