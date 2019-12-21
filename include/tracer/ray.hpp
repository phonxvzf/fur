#ifndef TRACER_RAY_HPP
#define TRACER_RAY_HPP

#include <limits>

#include "material.hpp"

namespace tracer {

  using namespace math;

  class ray {
    public:
      point3f   origin;
      vector3f  dir, inv_dir;
      Float     t_max;

      material::medium medium;

      ray() {}
      ray(const point3f& origin,
          const vector3f& dir,
          Float t_max = std::numeric_limits<Float>::max(),
          material::medium medium = OUTSIDE)
        : origin(origin), dir(dir), inv_dir(dir.inverse()),
        t_max(t_max), medium(medium) {}
      ray(const ray& cpy)
        : origin(cpy.origin), dir(cpy.dir), inv_dir(cpy.inv_dir),
        t_max(cpy.t_max), medium(cpy.medium) {}

      ray& operator=(const ray& cpy) {
        origin  = cpy.origin;
        dir     = cpy.dir;
        inv_dir = cpy.inv_dir;
        t_max   = cpy.t_max;
        medium  = cpy.medium;
        return *this;
      }

      point3f operator()(Float t) const {
        return origin + dir * t;
      }

      ray normalized() const {
        return ray(origin, dir.normalized(), t_max, medium);
      }
  };
}

#endif /* TRACER_RAY_HPP */
