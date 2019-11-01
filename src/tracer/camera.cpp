#include "tracer/camera.hpp"
#include "math/transform.hpp"
#include <iostream>

namespace tracer {
  namespace camera {

    using namespace math;

    projective::projective(
        const tf::transform& cam_to_world,
        const tf::transform& cam_to_ndc,
        const vector2i& img_res,
        const vector2f& ndc_res)
      : camera(cam_to_world), img_res(img_res), ndc_res(ndc_res),
      tf_cam_to_ndc(cam_to_ndc),
      tf_ndc_to_raster(tf::ndc_to_raster(img_res, ndc_res)),
      tf_raster_to_ndc(tf_ndc_to_raster.inverse()),
      tf_raster_to_cam(tf_cam_to_ndc.inverse() * tf_raster_to_ndc) {}

    ortho::ortho(
        const tf::transform& cam_to_world,
        const vector2i& img_res,
        const vector2f& ndc_res,
        Float near,
        Float far
        )
      : projective(cam_to_world, tf::ortho(near, far), img_res, ndc_res),
      near(near), far(far) {}

    ray ortho::generate_ray(const point2f& img_point) const {
      const point3f origin = tf_raster_to_cam(point3f(img_point));
      const vector3f dir(0.0, 0.0, 1.0);
      Float t_max = far - near;
      return tf_cam_to_world(ray(origin, dir, t_max));
    }

    persp::persp(
        const tf::transform& cam_to_world,
        const vector2i& img_res,
        const vector2f& ndc_res,
        Float near,
        Float far,
        Float fovy,
        Float aspect_ratio
        )
      : projective(cam_to_world, tf::persp(near, far, fovy, aspect_ratio), img_res, ndc_res),
      near(near), far(far), fovy(fovy), aspect_ratio(aspect_ratio) {}

    ray persp::generate_ray(const point2f& img_point) const {
      const point3f origin(tf_raster_to_cam(point3f(img_point)));
      return tf_cam_to_world(ray(origin, origin, (far - near) / std::cos(fovy / 2)));
    }
  }
}
