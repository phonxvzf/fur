#ifndef TRACER_EMBREE_ACCEL_HPP
#define TRACER_EMBREE_ACCEL_HPP

#include <memory>
#include <vector>
#include <embree3/rtcore.h>

#include "ray.hpp"
#include "shapes/cubic_bezier.hpp"

namespace tracer {
  class embree_accel {
    private:
      bool valid = false;
      RTCDevice embree_device;
      RTCScene embree_scene;

      const unsigned int curve_indices[4] = { 0, 1, 2, 3 };
      std::vector<std::shared_ptr<shapes::cubic_bezier>> beziers;

    public:
      typedef unsigned int geom_id;

      embree_accel();
      ~embree_accel();

      geom_id add_hair(const std::vector<std::shared_ptr<shape>>& curves);
      void commit();

      bool is_valid() const;
      bool intersect(const ray& r, shape::intersect_result* result) const;
      bool occluded(const ray& r) const;
  };
}

#endif /* TRACER_EMBREE_ACCEL_HPP */
