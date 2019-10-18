#ifndef TRACER_INTERSECTION_HPP
#define TRACER_INTERSECTION_HPP

#include "math/vector.hpp"

namespace tracer {

  using namespace math;

  struct intersect_opts {
    Float hit_epsilon     = 1e-4;
    Float normal_delta    = 1e-4;
    int trace_max_iters   = 1000;

    intersect_opts() : hit_epsilon(1e-4), normal_delta(1e-4), trace_max_iters(1000) {}
  };

  struct intersect_result {
    Float     t_hit;
    point3f   hit_point;
    normal3f  normal;
  };
}

#endif /* TRACER_INTERSECTION_HPP */
