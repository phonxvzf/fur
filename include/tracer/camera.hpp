#ifndef TRACER_CAMERA_HPP
#define TRACER_CAMERA_HPP

#include "math/transform.hpp"

namespace tracer {
  namespace camera {

    class camera { 
      protected:
        const tf::transform tf_cam_to_world;

      public:
        camera(const tf::transform& cam_to_world) : tf_cam_to_world(cam_to_world) {}

        virtual ray generate_ray(const point2f& img_point) const = 0;
    };

    class projective : public camera {
      protected:
        const vector2i img_res;
        const vector2i ndc_res;
        const tf::transform tf_cam_to_ndc;
        const tf::transform tf_ndc_to_raster, tf_raster_to_ndc;
        const tf::transform tf_raster_to_cam;

      public:
        vector3f position;

        projective(
            const tf::transform& cam_to_world,
            const tf::transform& cam_to_ndc,
            const vector2i& img_res,
            const vector2f& ndc_res
            );
    };

    class ortho : public projective {
      public:
        const Float near;
        const Float far;

        ortho(
            const tf::transform& cam_to_world,
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
        const Float aspect_ratio;

        persp(
            const tf::transform& cam_to_world,
            const vector2i& img_res,
            const vector2f& ndc_res,
            Float near,
            Float far,
            Float fovy,
            Float aspect_ratio
            );

        ray generate_ray(const point2f& img_point) const;
    };
  } /* namespace camera */
} /* namespace tracer */

#endif /* TRACER_CAMERA_HPP */
