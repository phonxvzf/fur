#include <unistd.h>
#include <thread>

#include "tracer/scene.hpp"
#include "math/random.hpp"

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
    rgb += Kd * std::max(Float(0), normal.dot(light_dir)) * light.color; // diffuse
    rgb += Ks * std::pow(std::max(Float(0), half.dot(normal)), Es) * light.color; // specular

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

  void scene::render_routine(const render_params& params, void (*update_callback)(Float)) {
    random::rng rng(params.seed);
    job j;
    while (master.get_job(&j)) {
      const vector2i start = j.start;
      const vector2i end = j.end;
      for (int y = start.y; y < end.y; ++y) {
        for (int x = start.x; x < end.x; ++x) {
          for (size_t n_samples = 0; n_samples < params.spp; ++n_samples) {

            rgb_spectrum rgb(0.0f);

            //const ray r = camera->generate_ray(point2f(x, y) + rng.next_2uf());
            const ray r = camera->generate_ray(point2f(x, y) + point2f(0.5f, 0.5f));
            const int i = params.img_res.x * y + x;

            const shape::intersect_result view_result
              = intersect_shapes(r, params.intersect_options);

            if (view_result.object != nullptr) {
              // viewing ray hits surface
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
                      params.intersect_options
                      );
                  if (shadow_result.object == nullptr) {
                    rgb += blinn_phong(
                        emitter,
                        view_result.hit_point,
                        view_result.object->surface.surface_rgb,
                        view_result.normal,
                        params.eye_position,
                        view_result.object->surface.Kd,
                        view_result.object->surface.Ks,
                        view_result.object->surface.Es
                        );
                  } else {
                    std::lock_guard<std::mutex> lock(shadow_counter_mutex);
                    ++shadow_counter;
                  }
                }
              }
              {
                std::lock_guard<std::mutex> lock(view_counter_mutex);
                ++view_counter;
              }
            } /* if view_result */

            ird_rgb->at(i) += (rgb / Float(params.spp)).clamp(0.0f, 1.0f);

          } /* for n_samples */

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
    } /* while get_job */
  }

  std::shared_ptr<std::vector<rgb_spectrum>> scene::render(
      const render_params& params,
      render_profile* profile,
      void (*update_callback)(Float)
      )
  {
    ird_rgb = std::make_shared<std::vector<rgb_spectrum>>(params.img_res.x * params.img_res.y);
    master.init(params.img_res, params.tile_size);
    
    last_update = std::chrono::system_clock::now();

    std::vector<std::thread> workers;
    for (size_t i = 0; i < params.n_workers; ++i) {
      workers.push_back(
          std::thread(&scene::render_routine, this, std::ref(params), update_callback)
          );
    }

    std::wcout << L"* Created " << params.n_workers << L" render workers" << std::endl;

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
