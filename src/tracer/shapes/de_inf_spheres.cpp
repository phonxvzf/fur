#include "tracer/shapes/de_inf_spheres.hpp"

namespace tracer {
  namespace shapes {
    de_inf_spheres::de_inf_spheres(
        const tf::transform& shape_to_world,
        const std::shared_ptr<material>& surface,
        Float radius,
        Float cell_size
        )
      : destimator(shape_to_world, surface), radius(radius), cell_size(cell_size) {}

    de_inf_spheres::de_inf_spheres(const de_inf_spheres& cpy)
      : destimator(cpy.tf_shape_to_world, cpy.surface),
      radius(cpy.radius),
      cell_size(cpy.cell_size) {}
        
    Float de_inf_spheres::sphere_position(Float x) const {
      const Float hs = cell_size / 2.0f;
      const int n_cells = x / cell_size;
      return (x < 0 ? -hs : hs) + cell_size * n_cells;
    }
        
    Float de_inf_spheres::distance_function(const point3f& p) const {
      Float dx = p.x - sphere_position(p.x);
      Float dy = p.y - sphere_position(p.y);
      Float dz = p.z - sphere_position(p.z);
      return std::sqrt(dx * dx + dy * dy + dz * dz) - radius;
    }
  }
}
