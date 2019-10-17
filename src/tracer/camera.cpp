#include "tracer/camera.hpp"
#include "math/transform.hpp"
#include <iostream>

namespace tracer {
  namespace camera {

    using namespace math;

    projective::projective(
        const matrix4f& cam_to_world,
        const matrix4f& cam_to_ndc,
        const vector2i& img_res,
        const vector2f& ndc_res)
      : camera(cam_to_world), tf_cam_to_ndc(cam_to_ndc), img_res(img_res), ndc_res(ndc_res)
    {
      tf_ndc_to_raster = tf::ndc_to_raster(img_res, ndc_res);
      tf_raster_to_ndc = tf_ndc_to_raster.inverse();
      tf_raster_to_cam = tf_cam_to_ndc.inverse() * tf_raster_to_ndc;
    }

    ortho::ortho(
        const matrix4f& cam_to_world,
        const vector2i& img_res,
        const vector2f& ndc_res,
        Float near,
        Float far
        )
      : projective(cam_to_world, tf::ortho(near, far), img_res, ndc_res),
      near(near), far(far) {}

    ray ortho::generate_ray(const point2f& img_point) const {
      const point3f origin = tf::apply(tf_raster_to_cam, img_point);
      const vector3f dir(0.0, 0.0, 1.0);
      return tf::apply(tf_cam_to_world, ray(origin, dir));
    }

    persp::persp(
        const matrix4f& cam_to_world,
        const vector2i& img_res,
        const vector2f& ndc_res,
        Float near,
        Float far,
        Float fovy
        )
      : projective(cam_to_world, tf::persp(near, far, fovy), img_res, ndc_res),
      near(near), far(far), fovy(fovy) {}

    ray persp::generate_ray(const point2f& img_point) const {
      const point3f origin(0);
      const vector3f dir(tf::apply(tf_raster_to_cam, img_point));
      return tf::apply(tf_cam_to_world, ray(origin, dir.normalized()));
    }
  }
}
