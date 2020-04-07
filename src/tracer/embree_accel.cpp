#include "tracer/embree_accel.hpp"

namespace tracer {
  embree_accel::embree_accel() {
    embree_device = rtcNewDevice(nullptr);
    embree_scene = rtcNewScene(embree_device);

    rtcSetSceneBuildQuality(embree_scene, RTC_BUILD_QUALITY_HIGH);
  }

  embree_accel::~embree_accel() {
    rtcReleaseScene(embree_scene);
    rtcReleaseDevice(embree_device);
  }

  embree_accel::geom_id embree_accel::add_hair(const std::vector<std::shared_ptr<shape>>& curves) {
    for (size_t i = 0; i < curves.size(); ++i)
      beziers.push_back(std::dynamic_pointer_cast<shapes::cubic_bezier>(curves[i]));

    for (size_t i = 0; i < curves.size(); ++i) {
      RTCGeometry geom = rtcNewGeometry(embree_device, RTC_GEOMETRY_TYPE_FLAT_BEZIER_CURVE);
      rtcSetGeometryVertexAttributeCount(geom, 1);

      // initialize curve indices
      rtcSetSharedGeometryBuffer(
          geom,
          RTC_BUFFER_TYPE_INDEX,
          0,
          RTC_FORMAT_UINT,
          curve_indices,
          0,
          sizeof(unsigned int),
          4
          );

      // initialize curve vertices
      vector4f* curve_vertices = (vector4f*) rtcSetNewGeometryBuffer(
          geom, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT4, sizeof(vector4f), 4
          );

      Float u = 0;
      for (size_t j = 0; j < 4; ++j) {
        curve_vertices[j] = beziers[i]->tf_shape_to_world(beziers[i]->control_points[j]);
        curve_vertices[j][3] = math::lerp(u, beziers[i]->thickness0, beziers[i]->thickness1);
        u += 1.f / 3;
      }

      // build geometry
      rtcSetGeometryUserData(geom, beziers[i].get());
      rtcSetGeometryBuildQuality(geom, RTC_BUILD_QUALITY_HIGH);
      rtcCommitGeometry(geom);
      if (rtcGetDeviceError(embree_device) != RTC_ERROR_NONE) {
        throw std::runtime_error("curve geometry malformed");
      }

      rtcAttachGeometry(embree_scene, geom);
      rtcReleaseGeometry(geom);
    }

    return curves.size();
  }

  void embree_accel::commit() {
    rtcCommitScene(embree_scene);
    if (rtcGetDeviceError(embree_device) != RTC_ERROR_NONE) {
      throw std::runtime_error("failed to commit scene");
    }
    valid = true;
  }

  bool embree_accel::is_valid() const {
    return valid;
  }

  bool embree_accel::intersect(const ray& r, shape::intersect_result* result) const {
    RTCIntersectContext intersect_ctx;
    RTCRayHit rtc_io;
    rtc_io.hit.geomID = RTC_INVALID_GEOMETRY_ID;
    rtc_io.ray.dir_x = r.dir.x;
    rtc_io.ray.dir_y = r.dir.y;
    rtc_io.ray.dir_z = r.dir.z;
    rtc_io.ray.org_x = r.origin.x;
    rtc_io.ray.org_y = r.origin.y;
    rtc_io.ray.org_z = r.origin.z;
    rtc_io.ray.tnear = 0.f;
    rtc_io.ray.tfar = r.t_max;
    rtc_io.ray.flags = 0;

    rtcInitIntersectContext(&intersect_ctx);
    rtcIntersect1(embree_scene, &intersect_ctx, &rtc_io);

    if (rtcGetDeviceError(embree_device) != RTC_ERROR_NONE) {
      throw std::runtime_error(std::to_string(rtcGetDeviceError(embree_device)));
    }

    if (rtc_io.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
      using namespace shapes;
      RTCGeometry geom = rtcGetGeometry(embree_scene, rtc_io.hit.geomID);
      const cubic_bezier* bezier =
        (const cubic_bezier*) rtcGetGeometryUserData(geom);
      result->object = bezier;
      result->t_hit = rtc_io.ray.tfar;
      result->hit_point = r(result->t_hit);
      result->uv = { rtc_io.hit.u, 0.5f * (rtc_io.hit.v + 1.f) };
      result->xbasis = cubic_bezier::evaluate_differential(
          result->uv[0], bezier->control_points
          ).normalized();
      const tf::transform rotate90 = tf::rotate(result->xbasis, PI_OVER_TWO);
      result->normal = bezier->tf_shape_to_world(rotate90(
            bezier->tf_world_to_shape(result->hit_point).cross(result->xbasis).normalized()
            ));
      result->xbasis = bezier->tf_shape_to_world(result->xbasis).normalized();
      if (r.medium == INSIDE) result->normal = -result->normal;
      return true;
    }

    return false;
  }

  bool embree_accel::occluded(const ray& r) const {
    RTCIntersectContext intersect_ctx;
    RTCRay rtc_ray;
    rtc_ray.dir_x = r.dir.x;
    rtc_ray.dir_y = r.dir.y;
    rtc_ray.dir_z = r.dir.z;
    rtc_ray.org_x = r.origin.x;
    rtc_ray.org_y = r.origin.y;
    rtc_ray.org_z = r.origin.z;
    rtc_ray.tnear = 0.f;
    rtc_ray.tfar = r.t_max;
    rtc_ray.flags = 0;

    rtcInitIntersectContext(&intersect_ctx);
    intersect_ctx.flags = RTC_INTERSECT_CONTEXT_FLAG_COHERENT;
    rtcOccluded1(embree_scene, &intersect_ctx, &rtc_ray);

    return rtc_ray.tfar < 0.f;
  }
} /* namespace tracer */
