#ifndef TRACER_SCENE_HPP
#define TRACER_SCENE_HPP

#include <vector>
#include <memory>
#include <mutex>
#include <thread>
#include <chrono>

#include "math/random.hpp"
#include "tracer/shape.hpp"
#include "tracer/light_source.hpp"
#include "tracer/camera.hpp"
#include "job_master.hpp"

namespace tracer {

  struct render_params {
    vector2i  img_res       = { 256, 256 };
    size_t    n_workers     = 1;
    size_t    spp           = 1;
    vector2i  tile_size     = vector2i(64, 64);
    uint64_t  seed          = 0;
    point3f   eye_position  = point3f(0.0f);
    bool      show_depth    = false;
    int       max_bounce    = 1;
    size_t    sspp          = 1;
    Float     max_rr        = 0.5;
    int       thread_id;

    shape::intersect_opts intersect_options = shape::intersect_opts();
  };

  struct render_profile {
    size_t time_elapsed;
  };

  class scene {
    private:
      void render_routine(
          const render_params& params,
          void (*update_callback)(Float, size_t)
          );

      shape::intersect_result intersect_shapes(
          const ray& r,
          const shape::intersect_opts& options,
          const tracer::shape* ignored_shape
          ) const;
  
      rgb_spectrum trace_path(
          const render_params& params,
          const ray& r,
          const tracer::shape* ignored_shape,
          const point2f& sample,
          int bounce
          );

      std::shared_ptr<std::vector<rgb_spectrum>> ird_rgb = nullptr;
      std::vector<light_source::emitter> light_emitters;
      std::vector<random::rng> rngs;

      Float inv_sspp;
      point2i n_strata;

      std::mutex pixel_counter_mutex;

      job_master master;

      // profiling
      size_t pixel_counter  = 0;

      static const uint32_t UPDATE_PERIOD = 1000; // ms

      std::chrono::system_clock::time_point last_update;
      std::chrono::system_clock::time_point render_start;

    public:
      // TODO: store shapes in a scene graph
      std::vector<std::shared_ptr<shape>> shapes;
      std::vector<std::shared_ptr<light_source>> light_sources;

      std::shared_ptr<camera::camera> camera;

      scene() {}
      scene(const scene& cpy)
        : shapes(cpy.shapes), light_sources(cpy.light_sources), camera(cpy.camera) {}

      std::shared_ptr<std::vector<rgb_spectrum>> render(
          const render_params& params,
          render_profile* profile = nullptr,
          void (*update_callback)(Float, size_t) = nullptr
          );
      float render_progress() const;
      bool rendering() const;
  };
} /* namespace tracer */

#endif /* TRACER_SCENE_HPP */