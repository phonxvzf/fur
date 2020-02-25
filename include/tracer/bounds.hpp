#ifndef TRACER_BOUNDS_HPP
#define TRACER_BOUNDS_HPP

#include "math/vector.hpp"
#include "ray.hpp"

namespace tracer {
  using namespace math;

  template <typename T> class bounds2 {
    public:
      point2<T> p_min, p_max;

      bounds2(const point2<T>& min, const point2<T>& max) {
        p_min = {
          std::min(min.x, max.x),
          std::min(min.y, max.y)
        };
        p_max = {
          std::max(min.x, max.x),
          std::max(min.y, max.y)
        };
      }

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

      bool contains(const point2<T>& p) const {
        return in_range(p.x, p_min.x, p_max.x) && in_range(p.y, p_min.y, p_max.y);
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
        vector2f min_dir(p_min - c), max_dir(p_max - c);
        return { c + s * min_dir, c + s * max_dir };
      }
  };

  template <typename T> class bounds3 {
    public:
      point3<T> p_min, p_max;

      bounds3(const point3<T>& min, const point3<T>& max) {
        p_min = {
          std::min(min.x, max.x),
          std::min(min.y, max.y),
          std::min(min.z, max.z)
        };
        p_max = {
          std::max(min.x, max.x),
          std::max(min.y, max.y),
          std::max(min.z, max.z)
        };
      }

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

      bool contains(const point3<T>& p) const {
        return in_range(p.x, p_min.x, p_max.x)
          && in_range(p.y, p_min.y, p_max.y)
          && in_range(p.z, p_min.z, p_max.z);
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

      bool intersect(const ray& r) const {
        Float t[6];
        t[0] = (p_min.x - r.origin.x) * r.inv_dir.x;
        t[1] = (p_max.x - r.origin.x) * r.inv_dir.x;
        t[2] = (p_min.y - r.origin.y) * r.inv_dir.y;
        t[3] = (p_max.y - r.origin.y) * r.inv_dir.y;
        t[4] = (p_min.z - r.origin.z) * r.inv_dir.z;
        t[5] = (p_max.z - r.origin.z) * r.inv_dir.z;
        Float t_min =
          std::max(std::max(std::min(t[0], t[1]), std::min(t[2], t[3])), std::min(t[4], t[5]));
        Float t_max =
          std::min(std::min(std::max(t[0], t[1]), std::max(t[2], t[3])), std::max(t[4], t[5]));
        if (t_max < 0 || t_min > t_max) return false;
        return true;
      }

      bounds3<T> scale(Float s) const {
        point3f c(centroid());
        vector3f min_dir(p_min - c), max_dir(p_max - c);
        return { c + s * min_dir, c + s * max_dir };
      }

      bounds3<T> expand(Float dx) const {
        return { p_min - vector3f(dx), p_max + vector3f(dx) };
      }

      point3f uvw(const point3f& p) const {
        vector3f d(diagonal());
        return {
          (p.x - p_min.x) / d.x,
          (p.y - p_min.y) / d.y,
          (p.z - p_min.z) / d.z
        };
      }
  };

  typedef bounds2<int> bounds2i;
  typedef bounds3<int> bounds3i;

  typedef bounds2<Float> bounds2f;
  typedef bounds3<Float> bounds3f;
}

#endif /* TRACER_BOUNDS_HPP */
