#ifndef TRACER_BOUNDS_HPP
#define TRACER_BOUNDS_HPP

#include "math/vector.hpp"
#include "ray.hpp"

namespace tracer {
  using namespace math;

  template <typename T> class bounds2 {
    public:
      point2<T> p_min, p_max;

      bounds2(const point2<T>& p_min, const point2<T>& p_max) : p_min(p_min), p_max(p_max) {}
      bounds2(const point2<T>& p = 0) : p_min(p), p_max(p) {}

      bounds2& operator=(const bounds2& cpy) {
        p_min = cpy.p_min;
        p_max = cpy.p_max;
        return *this;
      }

      T width_x() const {
        return p_max.x - p_min.x;
      }

      T width_y() const {
        return p_max.y - p_min.y;
      }

      T width() const {
        return width_x();
      }

      T height() const {
        return width_y();
      }

      T area() const {
        return width() * height();
      }

      point2f centroid() const {
        return 0.5f * (p_max + p_min);
      }

      vector2f diagonal() const {
        return p_max - p_min;
      }

      T longest() const {
        vector2f dd = diagonal();
        return std::max(dd.x, dd.y);
      }

      bool invalid() const {
        return width() <= 0 || height() <= 0;
      }

      bounds2<T> merge(const bounds2<T>& other) const {
        return bounds2(
            { std::min(p_min.x, other.p_min.x), std::min(p_min.y, other.p_min.y) },
            { std::max(p_max.x, other.p_max.x), std::max(p_max.y, other.p_max.y) }
            );
      }

      bounds2<T> intersect(const bounds2<T>& other) const {
        return bounds2(
            { std::max(p_min.x, other.p_min.x), std::max(p_min.y, other.p_min.y) },
            { std::min(p_max.x, other.p_max.x), std::min(p_max.y, other.p_max.y) }
            );
      }

      bounds2<T> scale(Float s) const {
        point2f c(centroid());
        vector2f min_dir(c - p_min), max_dir(p_max - c);
        return { c + s * min_dir, c + s * max_dir };
      }
  };

  template <typename T> class bounds3 {
    public:
      point3<T> p_min, p_max;

      bounds3(const point3<T>& p_min, const point3<T>& p_max) : p_min(p_min), p_max(p_max) {}
      bounds3(const point3<T>& p = 0) : p_min(p), p_max(p) {}

      bounds3& operator=(const bounds3& cpy) {
        p_min = cpy.p_min;
        p_max = cpy.p_max;
        return *this;
      }

      T width_x() const {
        return p_max.x - p_min.x;
      }

      T width_y() const {
        return p_max.y - p_min.y;
      }

      T width_z() const {
        return p_max.z - p_min.z;
      }

      T volume() const {
        return width_x() * width_y() * width_z();
      }

      point3f centroid() const {
        return 0.5f * (p_max + p_min);
      }

      bool invalid() const {
        return width_x() <= 0 || width_y() <= 0 || width_z() <= 0;
      }

      vector3f diagonal() const {
        return p_max - p_min;
      }

      T longest() const {
        vector3f dd = diagonal();
        return std::max(dd.x, std::max(dd.y, dd.z));
      }

      T which_longest() const {
        vector3f dd = diagonal();
        int i = 0;
        if (dd[0] > dd[i]) i = 0;
        if (dd[1] > dd[i]) i = 1;
        if (dd[2] > dd[i]) i = 2;
        return i;
      }

      T surface_area() const {
        vector3f dd = diagonal();
        return 2 * (dd.x * dd.y + dd.y * dd.z + dd.z * dd.x);
      }

      bounds3<T> merge(const bounds3<T>& other) const {
        return bounds3(
            {
            std::min(p_min.x, other.p_min.x),
            std::min(p_min.y, other.p_min.y),
            std::min(p_min.z, other.p_min.z)
            },
            {
            std::max(p_max.x, other.p_max.x),
            std::max(p_max.y, other.p_max.y),
            std::max(p_max.z, other.p_max.z)
            }
            );
      }

      bounds3<T> intersect(const bounds3<T>& other) const {
        return bounds3(
            {
            std::max(p_min.x, other.p_min.x),
            std::max(p_min.y, other.p_min.y),
            std::max(p_min.z, other.p_min.z)
            },
            {
            std::min(p_max.x, other.p_max.x),
            std::min(p_max.y, other.p_max.y),
            std::min(p_max.z, other.p_max.z)
            }
            );
      }

      bounds3<T> scale(Float s) const {
        point3f c(centroid());
        vector3f min_dir(c - p_min), max_dir(p_max - c);
        return { c + s * min_dir, c + s * max_dir };
      }
  };

  typedef bounds2<int> bounds2i;
  typedef bounds3<int> bounds3i;

  typedef bounds2<Float> bounds2f;
  typedef bounds3<Float> bounds3f;
}

#endif /* TRACER_BOUNDS_HPP */
