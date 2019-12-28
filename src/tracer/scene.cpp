#include <thread>

#include "tracer/scene.hpp"
#include "tracer/shapes/de_sphere.hpp"
#include "tracer/shapes/de_quad.hpp"
#include "tracer/material.hpp"
#include "tracer/materials/sss.hpp"
#include "math/random.hpp"
#include "math/util.hpp"
#include "math/pdf.hpp"
#include "math/sampler.hpp"

namespace tracer {

  rgb_spectrum scene::trace_path(
      const render_params& params,
      const ray& r,
      const material::light_transport& prev_lt,
      const point2f& sample,
      int bounce)
  {
    if (bounce > params.max_bounce) return rgb_spectrum(0);

    shape::intersect_result result;
    shapes.intersect(r, params.intersect_options, &result);

    if (result.object == nullptr) return rgb_spectrum(0);
    if (params.show_normal) return rgb_spectrum(result.normal.x, result.normal.y, result.normal.z);
    if (params.show_depth) return rgb_spectrum(result.t_hit);

    switch (result.object->surface->transport_model) {
      case material::EMIT:
        return result.object->surface->emittance;
      case material::NONE:
        return rgb_spectrum(0);
      default:
        break;
    }

    random::rng& rng = rngs[params.thread_id];

    vector3f u, v;
    sampler::sample_orthogonals(result.normal, &u, &v, rng);

    const matrix3f from_tangent_space(u, result.normal, v);

    // omegas in tangent space
    const vector3f omega_out(from_tangent_space.t().dot(-r.dir));
    vector3f omega_in;

    const material::light_transport next_lt =
      result.object->surface->sample(
          &omega_in,
          omega_out,
          { result.object->surface->transport_model, prev_lt.med },
          sample,
          rng.next_uf()
          );
    const vector3f bias(next_lt.med == INSIDE ? -result.normal : result.normal);
    ray r_next(
        result.hit_point + params.intersect_options.bias_epsilon * bias,
        from_tangent_space.dot(omega_in),
        r.t_max,
        next_lt.med
        );
    const rgb_spectrum color = (next_lt.transport == material::REFLECT) ?
      result.object->surface->rgb_refl : result.object->surface->rgb_refr;

    // do volumetric path tracing
    rgb_spectrum volume_weight(1.f);
    if (result.object->surface->transport_model == material::SSS && next_lt.med == INSIDE) {
      const auto volume = std::dynamic_pointer_cast<materials::sss>(result.object->surface);
      ASSERT(volume != nullptr);
      ray r_sss(r_next);

      shape::intersect_result sss_result;
      shapes.intersect(r_sss, params.intersect_options, &sss_result);

      bool hit = false;
      Float dist = volume->sample_distance(rng);
      r_sss.t_max = dist;
      while (!hit) {
        // reset intersect result
        sss_result = shape::intersect_result();
        hit = shapes.intersect(r_sss, params.intersect_options, &sss_result);

        ++bounce;

        if (bounce > params.max_bounce) return rgb_spectrum(0);
        if (rng.next_uf() < volume->absorp_prob) return rgb_spectrum(0);

        volume_weight *= volume->beta(true, dist);

        Float next_dist = volume->sample_distance(rng);
        r_sss = ray(
            r_sss.origin + r_sss.dir * dist,
            sampler::sample_henyey_greenstein(volume->g, rng.next_2uf()),
            next_dist,
            INSIDE);
        dist = next_dist;
      }

      volume_weight *= volume->beta(false, sss_result.t_hit);

      Float old_t_max = r_next.t_max;
      r_next = r_sss;
      r_next.t_max = old_t_max;
    }

    // russian roulette path termination
    const Float rr_prob = std::min(params.max_rr, 1 - color.max());
    if (rng.next_uf() < rr_prob) return rgb_spectrum(0);

    return volume_weight * (result.object->surface->emittance
      + result.object->surface->weight(omega_in, omega_out, next_lt)
      * trace_path(params, r_next, next_lt, sample, bounce + 1)
      / (1 - rr_prob));
  } /* trace_path() */

  void scene::render_routine(
      const render_params& params,
      void (*update_callback)(Float, size_t, size_t))
  {
    random::rng& rng = rngs[params.thread_id];
    job j;
    const Float inv_spp = Float(1) / params.spp;
    std::vector<point2f> stratified_samples;
    const size_t n_subpixels = (params.show_depth || params.show_normal) ? 1 : params.spp;

    while (master.get_job(&j)) {
      const vector2i start = j.start;
      const vector2i end = j.end;
      for (int y = start.y; y < end.y; ++y) {
        for (int x = start.x; x < end.x; ++x) {

          rgb_spectrum rgb_pixel(0.0f);
          const int i = params.img_res.x * y + x;

          for (size_t subpixel = 0; subpixel < n_subpixels; ++subpixel) {
            rgb_spectrum rgb(0.0f);
            const ray r = camera->generate_ray(point2f(x, y) + rng.next_2uf()).normalized();
            sampler::sample_stratified_2d(stratified_samples, params.sspp, n_strata, rng);
            for (const point2f& sample : stratified_samples) {
              rgb += trace_path(params, r, { material::REFLECT, OUTSIDE }, sample, 0);
            }
            rgb_pixel += rgb * inv_sspp;
          }

          ird_rgb->at(i) = rgb_pixel;

          if (!params.show_depth && !params.show_normal) {
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
              update_callback(render_progress(), eta, elapsed);
              last_update = system_clock::now();
            }
          }
        } /* for x */
      } /* for y */
    } /* while get_job */
  } /* render_routine() */

  std::shared_ptr<std::vector<rgb_spectrum>> scene::render(
      const render_params& params,
      render_profile* profile,
      void (*update_callback)(Float, size_t, size_t)
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

    std::wcout << L"  * Created " << params.n_workers << L" render workers" << std::endl;

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
  } /* render */

  float scene::render_progress() const {
    return static_cast<float>(pixel_counter) / ird_rgb->size();
  }
      
  bool scene::rendering() const {
    return ird_rgb->size() > pixel_counter;
  }
} /* namespace tracer */
