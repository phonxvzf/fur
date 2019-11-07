#ifndef TRACER_SCENE_HPP
#define TRACER_SCENE_HPP

#include <vector>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <boost/asio/thread_pool.hpp>

#include "tracer/shape.hpp"
#include "tracer/light_source.hpp"
#include "tracer/camera.hpp"

namespace tracer {

  struct render_params {
    vector2i img_res = { 256, 256 };
    size_t n_workers = 1;
    size_t spp = 1;
    uint64_t seed = 0;
    Float shadow_bias = 5e-4;
    point3f eye_position = point3f(0.0f);
    shape::intersect_opts intersect_options = shape::intersect_opts();
  };

  struct render_profile {
    size_t view_counter;
    size_t shadow_counter;
  };

  class scene {
    private:
      void render_routine(
          const render_params& params,
          const vector2i& start,
          const vector2i& end,
          void (*update_callback)(Float)
          );

      shape::intersect_result intersect_shapes(
          const ray& r,
          const shape::intersect_opts& options
          ) const;

      std::shared_ptr<std::vector<rgb_spectrum>> ird_rgb = nullptr;
      std::unique_ptr<boost::asio::thread_pool> worker_thread_pool = nullptr;

      std::mutex view_counter_mutex;
      std::mutex shadow_counter_mutex;
      std::mutex pixel_counter_mutex;
      std::mutex worker_semaphore_mutex;

      std::condition_variable worker_cond_var;

      int worker_semaphore;

      static const int TILE_WIDTH   = 64;
      static const int TILE_HEIGHT  = 64;

      // profiling
      size_t view_counter   = 0;
      size_t shadow_counter = 0;
      size_t pixel_counter  = 0;

      static const uint32_t UPDATE_PERIOD = 200; // ms

      std::chrono::system_clock::time_point last_update;

      bool workers_finished();

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
          void (*update_callback)(Float) = nullptr
          );
      float render_progress() const;
      bool rendering() const;
  };
} /* namespace tracer */

#endif /* TRACER_SCENE_HPP */
