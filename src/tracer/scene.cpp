#include <thread>
#include <stack>

#include "tracer/scene.hpp"
#include "math/random.hpp"
#include "math/util.hpp"
#include "math/pdf.hpp"
#include "math/sampler.hpp"

namespace tracer {

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

  rgb_spectrum scene::trace_path(const render_params& params, const ray& r, int bounce) const {
    if (bounce > params.max_bounce) return rgb_spectrum(0);

    shape::intersect_result result = intersect_shapes(r, params.intersect_options);
    if (result.object == nullptr) return rgb_spectrum(0);
    if (params.show_depth) return rgb_spectrum(result.t_hit);
    if (result.object->surface->transport == material::EMIT)
      return result.object->surface->emittance;

    vector3f u, v;
    orthogonals(result.normal, &u, &v);

    rgb_spectrum spectrum(0);

    random::rng rng(params.seed);
    const vector3f omega_out(-r.dir.normalized());

    for (const point2f& sample : stratified_samples) {
      const vector3f mf_normal = change_bases(
          sampler::sample_hemisphere(sample),
          u,
          result.normal,
          v
          ).normalized();
      const vector3f ray_dir(reflect(omega_out, mf_normal).normalized());
      const ray r_next(result.hit_point, ray_dir, r.t_max);
      const Float rr_prob = std::max(params.min_rr, maxdot(r_next.dir, result.normal));

      if (rng.next_uf() < rr_prob) {
        // for microfacets models
        spectrum += result.object->surface->weight(r_next.dir, mf_normal, result.normal)
          * trace_path(params, r_next, bounce + 1) / rr_prob;
      }
    }

    spectrum = spectrum * inv_sspp;

    return result.object->surface->emittance + spectrum;
  }

  void scene::render_routine(const render_params& params, void (*update_callback)(Float, size_t)) {
    random::rng rng(params.seed);
    job j;
    const Float inv_spp = Float(1) / params.spp;
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
            rgb_pixel += trace_path(params, r, 0);
          }

          ird_rgb->at(i) = rgb_pixel;

          if (!params.show_depth) {
            ird_rgb->at(i) = ird_rgb->at(i) * inv_spp;
          }

          // profile if requested
          if (update_callback != nullptr) {
            std::lock_guard<std::mutex> lock(pixel_counter_mutex);
            ++pixel_counter;

            using namespace std::chrono;
            if ((duration_cast<milliseconds>(system_clock::now() - last_update).count()
                  >= UPDATE_PERIOD) || !rendering())
            {
              size_t elapsed = duration_cast<seconds>(system_clock::now() - render_start).count();
              size_t remaining = params.img_res.x * params.img_res.y - pixel_counter;
              Float pps = elapsed > 0 ? pixel_counter / elapsed : 0;
              size_t eta = pps ? remaining / pps : 0;
              update_callback(render_progress(), eta);
              last_update = system_clock::now();
            }
          }
        } /* for x */
      } /* for y */
    } /* while get_job */
  }

  std::shared_ptr<std::vector<rgb_spectrum>> scene::render(
      const render_params& params,
      render_profile* profile,
      void (*update_callback)(Float, size_t)
      )
  {
    ird_rgb = std::make_shared<std::vector<rgb_spectrum>>(params.img_res.x * params.img_res.y);

    // pre-sample rays
    random::rng rng(params.seed);
    stratified_samples = sampler::sample_stratified_2d(
        params.sspp,
        point2i(std::max(1, static_cast<int>(std::sqrt(params.sspp)))),
        rng
        );
    inv_sspp = Float(1) / params.sspp;

    // init thread scheduler
    master.init(params.img_res, params.tile_size);
    
    last_update = std::chrono::system_clock::now();
    render_start = std::chrono::system_clock::now();

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

    // render finished
    if (profile) {
      profile->time_elapsed = std::chrono::duration_cast<std::chrono::seconds>(
          (std::chrono::system_clock::now() - render_start)
          ).count();
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
