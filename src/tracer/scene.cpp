#include <thread>

#include "tracer/scene.hpp"

namespace tracer {

  // TODO: remove this
  rgb_spectrum blinn_phong(
      const light_source::emitter& light,
      const point3f& surface_point,
      const rgb_spectrum& surface_rgb,
      const normal3f& normal,
      const point3f& eye,
      Float Kd,
      Float Ks,
      Float Es
      )
  {
    rgb_spectrum rgb(0.0f);
    vector3f eye_dir((eye - surface_point).normalized());
    const vector3f light_dir((light.position - surface_point).normalized());
    const vector3f half((light_dir + eye_dir).normalized());
    rgb += Kd * std::max(0.0f, normal.dot(light_dir)) * light.color; // diffuse
    rgb += Ks * std::pow(std::max(0.0f, half.dot(normal)), Es) * light.color; // specular

    // assume uniform light
    return rgb * surface_rgb;
  }

  shape::intersect_result scene::intersect_shapes(
      const ray& r,
      const shape::intersect_opts& options
      ) const
  {
    // TODO: scene graph, BVH
    Float t_min = std::numeric_limits<Float>::max();
    shape::intersect_result result_seen;
    for (const std::shared_ptr<shape>& object : shapes) {
      shape::intersect_result result;
      if (object->intersect(r, options, &result)) {
        if (result.t_hit < t_min) {
          t_min = result.t_hit;
          result_seen = result;
        }
      }
    }
    return result_seen;
  }

  void scene::render_routine(
      const render_params& params,
      const vector2i& img_res,
      const vector2i& start,
      const vector2i& end,
      void (*update_callback)(Float)
      )
  {
    for (int y = start.y; y < end.y; ++y) {
      for (int x = start.x; x < end.x; ++x) {

        // only sample 1 ray for now (at the center of each pixel)
        const ray r = camera->generate_ray(point2f(x + 0.5f, y + 0.5f));
        const int i = img_res.x * y + x;

        const shape::intersect_result view_result = intersect_shapes(r, shape::intersect_opts());

        if (view_result.object != nullptr) {
          // viewing ray hits surface
          rgb_spectrum rgb(0.0f);

          // cast shadow rays
          shape::intersect_result shadow_result;
          for (const std::shared_ptr<light_source>& light : light_sources) {

            std::vector<light_source::emitter> light_emitters(light->sample_lights());

            for (const light_source::emitter& emitter : light_emitters) {
              const point3f shadow_r_origin(
                  view_result.hit_point + params.shadow_bias * view_result.normal
                  );
              const vector3f shadow_r_dir(emitter.position - shadow_r_origin);
              const ray shadow_r(
                  shadow_r_origin,
                  shadow_r_dir,
                  shadow_r_dir.size()
                  );
              const shape::intersect_result shadow_result = intersect_shapes(
                  shadow_r,
                  shape::intersect_opts()
                  );
              if (shadow_result.object == nullptr) {
                rgb += blinn_phong(
                    emitter,
                    view_result.hit_point,
                    params.surface_rgb,
                    view_result.normal,
                    params.eye_position,
                    params.Kd,
                    params.Ks,
                    params.Es
                    );
              } else {
                std::lock_guard<std::mutex> lock(shadow_counter_mutex);
                ++shadow_counter;
              }
            }
          }

          ird_rgb->at(i) += rgb.clamp(0.0f, 1.0f);

          {
            std::lock_guard<std::mutex> lock(view_counter_mutex);
            ++view_counter;
          }
        } /* if */

        // profile if requested
        {
          std::lock_guard<std::mutex> lock(pixel_counter_mutex);
          ++pixel_counter;

          if (update_callback != nullptr) {
            using namespace std::chrono;
            if ((duration_cast<milliseconds>(system_clock::now() - last_update).count()
                >= UPDATE_PERIOD) || !rendering())
            {
              update_callback(render_progress());
              last_update = system_clock::now();
            }
          }
        }
      } /* for x */
    } /* for y */
  }

  std::shared_ptr<std::vector<rgb_spectrum>> scene::render(
      const render_params& params,
      const vector2i& img_res,
      size_t n_workers,
      render_profile* profile,
      void (*update_callback)(Float)
      )
  {
    // TODO: efficient thread pooling, minimize core idle time
    ird_rgb = std::make_shared<std::vector<rgb_spectrum>>(img_res.x * img_res.y);

    last_update = std::chrono::system_clock::now();

    std::vector<std::thread> workers;
    const Float tile_height = img_res.y / n_workers;
    for (size_t i = 0; i < n_workers; ++i) {
      const vector2i start(0, i * tile_height);
      vector2i end(img_res.x, start.y + tile_height);
      if ((i + 1 == n_workers) && (end.y < img_res.y)) {
        end.y = img_res.y;
      }
      workers.push_back(std::thread(
            &scene::render_routine, this, params, img_res, start, end, update_callback)
          );
    }

    std::wcout << L"* Created " << workers.size() << L" render workers" << std::endl;
    for (std::thread& worker : workers) {
      worker.join();
    }

    if (profile) {
      profile->view_counter   = view_counter;
      profile->shadow_counter = shadow_counter;
    }

    return ird_rgb;
  }

  float scene::render_progress() const {
    return static_cast<float>(pixel_counter) / ird_rgb->size();
  }
      
  bool scene::rendering() const {
    return ird_rgb->size() > pixel_counter;
  }
} /* namespace tracer */
