#ifndef TRACER_CAMERA_HPP
#define TRACER_CAMERA_HPP

#include "math/matrix.hpp"
#include "ray.hpp"

namespace tracer {
  namespace camera {

    class camera { 
      protected:
        const matrix4f tf_cam_to_world;

      public:
        camera(const matrix4f& cam_to_world) : tf_cam_to_world(cam_to_world) {}
    };

    class projective : public camera {
      protected:
        matrix4f tf_cam_to_ndc;
        matrix4f tf_ndc_to_raster, tf_raster_to_ndc;
        matrix4f tf_raster_to_cam;
        const vector2i img_res;
        const vector2i ndc_res;

      public:
        vector3f position;

        projective(
            const matrix4f& cam_to_world,
            const matrix4f& cam_to_ndc,
            const vector2i& img_res,
            const vector2f& ndc_res
            );

        virtual ray generate_ray(const point2f& img_point) const = 0;
    };

    class ortho : public projective {
      public:
        const Float near;
        const Float far;

        ortho(
            const matrix4f& cam_to_world,
            const vector2i& img_res,
            const vector2f& ndc_res,
            Float near,
            Float far
            );

        ray generate_ray(const point2f& img_point) const;
    };

    class persp : public projective {
      public:
        const Float near;
        const Float far;
        const Float fovy;

        persp(
            const matrix4f& cam_to_world,
            const vector2i& img_res,
            const vector2f& ndc_res,
            Float near,
            Float far,
            Float fovy
            );

        ray generate_ray(const point2f& img_point) const;
    };
  } /* namespace camera */
} /* namespace tracer */

#endif /* TRACER_CAMERA_HPP */
