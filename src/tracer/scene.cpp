#include <thread>
#include <stack>

#include "tracer/scene.hpp"
#include "tracer/shapes/de_sphere.hpp"
#include "tracer/shapes/de_quad.hpp"
#include "math/random.hpp"
#include "math/util.hpp"
#include "math/pdf.hpp"
#include "math/sampler.hpp"

namespace tracer {

  shape::intersect_result scene::intersect_shapes(
      const ray& r,
      const shape::intersect_opts& options,
      const tracer::shape* ignored_shape
      ) const
  {
    // TODO: scene graph, BVH
    Float t_min = std::numeric_limits<Float>::max();
    shape::intersect_result result_seen;
    for (const std::shared_ptr<shape>& object : shapes) {
      if (object.get() == ignored_shape) continue;
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

  rgb_spectrum scene::trace_path(
      const render_params& params,
      const ray& r,
      const tracer::shape* last_shape,
      const point2f& sample, // TODO: use stratified samples
      int bounce)
  {
    if (bounce > params.max_bounce) return rgb_spectrum(0);

    shape::intersect_result result = intersect_shapes(r, params.intersect_options, last_shape);

    if (result.object == nullptr) return rgb_spectrum(0);
    if (params.show_depth) return rgb_spectrum(result.t_hit);
    if (result.object->surface->transport == material::EMIT)
      return result.object->surface->emittance;

    random::rng& rng = rngs[params.thread_id];

    vector3f u, v;
    sampler::sample_orthogonals(result.normal, &u, &v, rng);

    const matrix3f from_tangent_space(u, result.normal, v);

    // omegas in tangent space
    const vector3f omega_out(from_tangent_space.t().dot(-r.dir));
    const vector3f omega_in = result.object->surface->sample(omega_out, sample);
    
    if ((omega_in + omega_out).is_zero()) return rgb_spectrum(0);

    const ray r_next(result.hit_point, from_tangent_space.dot(omega_in), r.t_max);

    // Russian roulette path termination
    const Float rr_prob = std::min(params.max_rr, 1 - result.object->surface->surface_rgb.max());
    if (rng.next_uf() < rr_prob) return rgb_spectrum(0);

    return result.object->surface->emittance
      + result.object->surface->weight(omega_in, omega_out)
      * trace_path(params, r_next, result.object, sample, bounce + 1) / (1 - rr_prob);
  }

  void scene::render_routine(const render_params& params, void (*update_callback)(Float, size_t)) {
    random::rng& rng = rngs[params.thread_id];
    job j;
    const Float inv_spp = Float(1) / params.spp;
    std::vector<point2f> stratified_samples;

    while (master.get_job(&j)) {
      const vector2i start = j.start;
      const vector2i end = j.end;
      for (int y = start.y; y < end.y; ++y) {
        for (int x = start.x; x < end.x; ++x) {

          rgb_spectrum rgb_pixel(0.0f);
          const int i = params.img_res.x * y + x;

          for (size_t n_samples = 0; n_samples < params.spp; ++n_samples) {
            rgb_spectrum rgb(0.0f);
            const ray r = camera->generate_ray(point2f(x, y) + rng.next_2uf()).normalized();
            sampler::sample_stratified_2d(stratified_samples, params.sspp, n_strata, rng);
            for (const point2f& sample : stratified_samples) {
              rgb += trace_path(params, r, nullptr, sample, 0);
            }
            rgb_pixel += rgb * inv_sspp;
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
    for (size_t i = 0; i < params.n_workers; ++i) {
      rngs.push_back(random::rng(params.seed + i));
    }

    ird_rgb = std::make_shared<std::vector<rgb_spectrum>>(params.img_res.x * params.img_res.y);
    inv_sspp = Float(1) / params.sspp;
    n_strata = point2i(std::max(1, static_cast<int>(std::sqrt(params.sspp))));

    // init thread scheduler
    master.init(params.img_res, params.tile_size);

    last_update = std::chrono::system_clock::now();
    render_start = std::chrono::system_clock::now();

    // render
    std::vector<std::thread> workers;
    for (size_t i = 0; i < params.n_workers; ++i) {
      render_params thread_params(params);
      thread_params.thread_id = i;
      workers.push_back(
          std::thread(&scene::render_routine, this, thread_params, update_callback)
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
