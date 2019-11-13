#include <thread>

#include "tracer/scene.hpp"
#include "math/random.hpp"
#include "math/util.hpp"
#include "math/pdf.hpp"
#include "math/sampler.hpp"

namespace tracer {

  Float geom_ggx(const normal3f& normal, const vector3f& omega, Float alpha2) {
    Float n_dot_omega = std::max(Float(0), normal.dot(omega));
    return 2 * n_dot_omega
      / (n_dot_omega + std::sqrt(n_dot_omega * n_dot_omega * (1 - alpha2) + alpha2));
  }

  rgb_spectrum brdf(
      const vector3f& omega_in,
      const vector3f& omega_out,
      const normal3f& normal)
  {
    const rgb_spectrum F(0.95, 0.93, 0.88); // use silver for testing
    //const rgb_spectrum F(1.00, 0.71, 0.29); // use gold for testing
    const Float alpha = 0.08; // alpha = roughness2
    const vector3f half = (omega_in + omega_out).normalized();

    const Float n_dot_half = std::max(Float(0), normal.dot(half));
    const Float alpha2 = alpha * alpha;
    Float denom = 1 + n_dot_half * n_dot_half * (alpha2 - 1);
    const Float ggx = alpha2 / (MATH_PI * denom * denom);

    // bi-directional geometry term
    const Float geom = 1 /
      (1 + geom_ggx(normal, omega_out, alpha2) + geom_ggx(normal, omega_in, alpha2));

    const rgb_spectrum fresnel = F + (rgb_spectrum(1) - F)
      * std::pow(1 - std::max(Float(0), omega_in.dot(half)), 5);

    denom = std::max(Float(0), 4 * normal.dot(omega_out) * normal.dot(omega_in));
    if (COMPARE_EQ(denom, 0)) return rgb_spectrum(0);

    // Torrance-Sparrow
    return ggx * fresnel * geom / denom;
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
        if (result.t_hit > 0 && result.t_hit < t_min) {
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

          rgb_spectrum rgb_pixel(0.0f);
          const int i = params.img_res.x * y + x;

          for (size_t n_samples = 0; n_samples < params.spp; ++n_samples) {

            rgb_spectrum rgb(0.0f);
            const ray r = camera->generate_ray(point2f(x, y) + rng.next_2uf());

            const shape::intersect_result view_result
              = intersect_shapes(r, params.intersect_options);

            if (view_result.object != nullptr) {
              if (!params.show_depth) {
                // viewing ray hits surface
                // cast shadow rays
                shape::intersect_result shadow_result;
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
                    // shadow ray did not intersect any object
                    const vector3f omega_in = shadow_r_dir.normalized();
                    const vector3f omega_out =
                      (params.eye_position - view_result.hit_point).normalized();
                    const Float dot = std::max(Float(0), omega_in.dot(view_result.normal));
                    const Float prob = emitter.parent->pdf(view_result.hit_point, emitter);
                    if (!COMPARE_EQ(prob, 0)) {
                      rgb += (
                          view_result.object->surface.Kd * view_result.object->surface.surface_rgb
                          * INV_PI
                          + brdf(omega_in, omega_out, view_result.normal)
                          )
                        * emitter.color
                        * dot
                        / prob;
                    }
                  } else {
                    std::lock_guard<std::mutex> lock(shadow_counter_mutex);
                    ++shadow_counter;
                  }
                } /* for emitter */
                if (!light_emitters.empty()) {
                  rgb = rgb / light_emitters.size();
                }
              } /* if show_depth */
              {
                std::lock_guard<std::mutex> lock(view_counter_mutex);
                ++view_counter;
              }
            } /* if view_result */

            if (params.show_depth) {
              if (view_result.object == nullptr) {
                ird_rgb->at(i) = rgb_spectrum(-1);
              } else {
                ird_rgb->at(i) = rgb_spectrum(view_result.t_hit);
              }
            }

            rgb_pixel += rgb;
          } /* for n_samples */

          if (!params.show_depth) {
            ird_rgb->at(i) = rgb_pixel / params.spp;
          }

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

    // pre-sample lights
    random::rng rng(params.seed);
    size_t n_samples = 0;
    for (const std::shared_ptr<light_source>& light : light_sources) {
      n_samples += light->spp;
    }

    ASSERT(n_samples > 0);
    const point2i n_strata(44, 44);
    std::vector<point2f> stratifed_samples(std::max(size_t(n_strata.x * n_strata.y), n_samples));
    sampler::sample_stratified_2d(stratifed_samples, stratifed_samples.size(), n_strata, rng);
    for (const std::shared_ptr<light_source>& light : light_sources) {
      int sample_i = 0;
      for (size_t i = 0; i < light->spp; ++i) {
        light_emitters.push_back(light->sample(stratifed_samples[sample_i]));
        sample_i = (sample_i + 1) % stratifed_samples.size();
      }
    }

    // init thread scheduler
    master.init(params.img_res, params.tile_size);
    
    last_update = std::chrono::system_clock::now();

    // render
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
