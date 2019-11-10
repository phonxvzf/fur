#include <thread>

#include "tracer/scene.hpp"
#include "math/random.hpp"
#include "math/util.hpp"
#include "math/pdf.hpp"

namespace tracer {

  Float geom_ggx(const normal3f& normal, const vector3f& omega, Float k) {
    Float n_dot_omega = std::max(Float(0), normal.dot(omega));
    return n_dot_omega / (k + n_dot_omega * (1 - k));
  }

  rgb_spectrum brdf(const vector3f& omega_in, const vector3f& omega_out, const normal3f& normal) {
    const rgb_spectrum F(0.5);
    const Float alpha = 0.2;
    const vector3f half = (omega_in + omega_out).normalized();

    const Float n_dot_half = std::max(Float(0), normal.dot(half));
    const Float alpha2 = alpha * alpha;
    Float denom = 1 + n_dot_half * n_dot_half * (alpha2 - 1);
    const Float ggx = alpha2 / (MATH_PI * denom * denom);

    const Float k = (alpha + 1) * (alpha + 1) / 8;
    const Float geom = geom_ggx(normal, omega_out, k) * geom_ggx(normal, omega_in, k);

    const rgb_spectrum fresnel = F + (rgb_spectrum(1) - F)
      * std::pow(1 - std::max(Float(0), omega_out.dot(half)), 5);

    denom = 4 * normal.dot(omega_out) * normal.dot(omega_in);
    if (COMPARE_EQ(denom, 0)) return rgb_spectrum(0);
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
                int n_emitters_seen = 0;
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
                    const Float prob =
                      pdf_uniform_cosine_hemisphere_cos(view_result.normal.dot(omega_in));
                    if (!COMPARE_EQ(prob, 0)) {
                      rgb += brdf(omega_in, omega_out, view_result.normal)
                        * emitter.color
                        * std::max(Float(0), omega_in.dot(view_result.normal))
                        / prob;
                    }
                    ++n_emitters_seen;
                  } else {
                    std::lock_guard<std::mutex> lock(shadow_counter_mutex);
                    ++shadow_counter;
                  }
                } /* for emitter */
                if (n_emitters_seen > 0) rgb = rgb / n_emitters_seen;
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
    for (const std::shared_ptr<light_source>& light : light_sources) {
      // TODO: light spp in YAML
      for (int i = 0; i < 1000; ++i) {
        light_emitters.push_back(light->sample_light(rng.next_2uf()));
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
