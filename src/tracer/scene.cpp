#include <thread>

#include "tracer/scene.hpp"
#include "tracer/shapes/de_sphere.hpp"
#include "tracer/shapes/de_quad.hpp"
#include "tracer/shapes/cubic_bezier.hpp"
#include "tracer/material.hpp"
#include "tracer/materials/sss.hpp"
#include "math/random.hpp"
#include "math/util.hpp"
#include "math/pdf.hpp"
#include "math/sampler.hpp"

namespace tracer {

  bool scene::intersect(
      const ray& r,
      const shape::intersect_opts& opts,
      shape::intersect_result* result
      ) const
  {
    bool hit = legacy_shapes.intersect(r, opts, result);
    if (embree_shapes.is_valid()) {
      shape::intersect_result embree_result;
      hit |= embree_shapes.intersect(r, &embree_result);
      if (embree_result.t_hit < result->t_hit) *result = embree_result;
    }
    return hit;
  }

  bool scene::occluded(const ray& r, const shape::intersect_opts& opts) const {
    bool hit = legacy_shapes.occluded(r, opts);
    if (embree_shapes.is_valid()) hit |= embree_shapes.occluded(r);
    return hit;
  }

  material::light_transport scene::trace_bsdf(
      ray* r_next,
      vector3f* omega_in,
      vector3f* omega_out,
      normal3f* mf_normal,
      vector3f* omega_in_dl,
      Float* pdf,
      Float* pdf_dl,
      sampled_spectrum* direct_light,
      const shape::intersect_result& result,
      const render_params& params,
      const ray& r,
      const material::light_transport& prev_lt,
      const point2f& sample,
      random::rng& rng
      )
  {
    const normal3f normal(result.normal.dot(r.dir) > 0 ? -result.normal : result.normal);

    vector3f u, v;
    sampler::sample_orthogonals(normal, &u, &v, rng);
    matrix3f from_tangent_space(u, normal, v);

    const shapes::cubic_bezier* curve = nullptr;
    if (result.object->surface->transport_model == material::HAIR) {
      curve = dynamic_cast<const shapes::cubic_bezier*>(result.object);
      if (curve != nullptr) {
        vector3f zbasis = result.xbasis.cross(normal).normalized();
        from_tangent_space = matrix3f(result.xbasis, normal, zbasis);
        *mf_normal = vector3f(result.uv);
      }
    }

    matrix3f to_tangent_space(from_tangent_space.t());

    // vectors in tangent space
    *omega_out = to_tangent_space.dot(-r.dir);

    // sample incident direction and microsurface (microfacet) normal
    Float xi = rng.next_uf();
    const material::light_transport next_lt =
      result.object->surface->sample(
          omega_in,
          mf_normal,
          pdf,
          *omega_out,
          { result.object->surface->transport_model, prev_lt.med },
          { sample.x, sample.y, xi }
          );
    const vector3f bias(next_lt.med == INSIDE ? -result.normal : result.normal);

    if (curve != nullptr && omega_in->y < 0) {
      Float hair_thickness = math::lerp(result.uv[0], curve->thickness0, curve->thickness1);
      Float offset = hair_thickness + params.intersect_options.bias_epsilon;
      vector3f world_omega_in = from_tangent_space.dot(*omega_in);
      *r_next = ray(
          result.hit_point + offset * world_omega_in,
          world_omega_in,
          r.t_max,
          next_lt.med
          );
    } else {
      *r_next = ray(
          result.hit_point + params.intersect_options.bias_epsilon * bias,
          from_tangent_space.dot(*omega_in),
          r.t_max,
          next_lt.med
          );
    }

    // sample direct lighting
    if (params.mis) {
      const point3f light_position = direct_light_shape->sample(sample);
      const ray r_dl(
          r_next->origin,
          (light_position - result.hit_point).normalized(),
          r.t_max
          );
      if (occluded(r_dl, params.intersect_options)) {
        *pdf_dl = 0;
      } else {
        *pdf_dl = direct_light_shape->pdf();
        *omega_in_dl = to_tangent_space.dot(r_dl.dir);
        *direct_light = direct_light_shape->surface->emittance
          / (light_position - result.hit_point).size_sq();
      }
    }

    return next_lt;
  }

  sampled_spectrum scene::estimate_radiance(
      const render_params& params,
      const vector3f& omega_in,
      const vector3f& omega_out,
      const normal3f& mf_normal,
      const normal3f& omega_in_dl,
      const material::light_transport& next_lt,
      const shape::intersect_result& result,
      const sampled_spectrum& direct_light,
      Float pdf,
      Float pdf_dl
      ) const
  {
    sampled_spectrum bxdf_radiance = result.object->surface->bxdf(
        omega_in, omega_out, mf_normal, next_lt
        );
    sampled_spectrum direct_radiance = result.object->surface->bxdf(
        omega_in_dl, omega_out, mf_normal, next_lt
        );
    Float bxdf_weight = balance_heuristic(params.spp, pdf, params.spp, pdf_dl);
    Float direct_weight = balance_heuristic(params.spp, pdf_dl, params.spp, pdf);
    sampled_spectrum bxdf_estimator = COMPARE_EQ(pdf, 0) ?
      sampled_spectrum(0.f)
      : (bxdf_weight * std::abs(omega_in.y)) * bxdf_radiance / pdf;
    sampled_spectrum direct_estimator = COMPARE_EQ(pdf_dl, 0) ?
      sampled_spectrum(0.f)
      : (direct_weight * std::abs(omega_in_dl.y)) * direct_radiance * direct_light / pdf_dl;
    return bxdf_estimator + direct_estimator;
  }

  nspectrum scene::trace_path(
      const render_params& params,
      const ray& r,
      const material::light_transport& prev_lt,
      const point2f& sample,
      random::rng& rng,
      int bounce)
  {
    if (bounce > params.max_bounce) return sampled_spectrum(0.f);

    shape::intersect_result result;
    intersect(r, params.intersect_options, &result);

    if (result.object == nullptr) {
      if (r.medium == OUTSIDE) {
        if (environment_texture != nullptr) {
          return environment_texture->sample(r.dir);
        }
        return environment_color;
      }
      return sampled_spectrum(0);
    }

    if (params.show_normal) return rgb_spectrum(result.normal.x, result.normal.y, result.normal.z);
    if (params.show_depth) return rgb_spectrum(result.t_hit);

    switch (result.object->surface->transport_model) {
      case material::EMIT:
        return result.object->surface->emittance;
      case material::NONE:
        return sampled_spectrum(0);
      default:
        break;
    }

    // evaluate bsdf
    sampled_spectrum weight0(1.), weight1(1.); // bsdf weights
    sampled_spectrum direct_light(1.f);
    ray r_next;
    vector3f omega_in, omega_out;
    vector3f omega_in_dl;
    normal3f mf_normal;
    Float pdf = 1.f, pdf_dl = 0.f;

    material::light_transport next_lt = trace_bsdf(
        &r_next, &omega_in, &omega_out, &mf_normal, &omega_in_dl,
        &pdf, &pdf_dl, &direct_light, result, params, r, prev_lt, sample, rng
        );

    // incoming bsdf
    weight0 = estimate_radiance(params, omega_in, omega_out, mf_normal, omega_in_dl,
        next_lt, result, direct_light, pdf, pdf_dl);

    // do volumetric path tracing
    sampled_spectrum volume_weight(1.f);
    if (result.object->surface->transport_model == material::SSS && next_lt.med == INSIDE) {
      const auto volume = std::dynamic_pointer_cast<materials::sss>(result.object->surface);
      ASSERT(volume != nullptr);

      bvh_tree* bvh = &legacy_shapes;

      // if the shape is a part of hair segment, only search in the strand
      auto curve = dynamic_cast<const shapes::cubic_bezier*>(result.object);
      if (curve != nullptr) {
        if (curve->hair_id != 0) {
          bvh = &strand_bvh[curve->hair_id][curve->strand_id];
        }
      }

      ray r_sss(r_next);

      shape::intersect_result sss_result;
      bvh->intersect(r_sss, params.intersect_options, &sss_result);

      bool hit = false;
      Float dist = volume->sample_distance(rng);
      r_sss.t_max = dist;
      while (!hit) {
        // reset intersect result
        sss_result = shape::intersect_result();

        hit = bvh->intersect(r_sss, params.intersect_options, &sss_result);

        if (++bounce > params.max_bounce) return sampled_spectrum(0);

        sampled_spectrum tr = volume->transmittance(dist);
        sampled_spectrum density = volume->density(tr, true);
        Float p = volume->pdf(density);

        if (COMPARE_EQ(p, 0)) return sampled_spectrum(0);
        volume_weight *= volume->beta(tr, true) / p;

        Float next_dist = volume->sample_distance(rng);

        vector3f basis0, basis1;
        sampler::sample_orthogonals(r_sss.dir, &basis0, &basis1, rng);
        matrix3f prev_space(basis0, r_sss.dir, basis1);
        vector3f dir(
            prev_space.dot(-sampler::sample_henyey_greenstein(volume->g, rng.next_2uf()))
            );

        r_sss = ray(
            r_sss.origin + r_sss.dir * dist,
            dir,
            next_dist,
            INSIDE);
        dist = next_dist;
      } /* while !hit */

      // ray comes out of medium
      if (++bounce > params.max_bounce) return sampled_spectrum(0);
      sss_result = shape::intersect_result();
      r_sss.t_max = r_next.t_max;
      hit = intersect(r_sss, params.intersect_options, &sss_result);
      if (!hit) return sampled_spectrum(0);
      sampled_spectrum tr = volume->transmittance(sss_result.t_hit);
      sampled_spectrum density = volume->density(tr, false);
      Float p = volume->pdf(density);

      if (COMPARE_EQ(p, 0)) return sampled_spectrum(0);
      volume_weight *= volume->beta(tr, false) / p;

      next_lt = trace_bsdf(
          &r_sss, &omega_in, &omega_out, &mf_normal, &omega_in_dl, &pdf, &pdf_dl, &direct_light,
          sss_result, params, r_sss, { material::REFRACT, INSIDE }, sample, rng
          );

      // outgoing btdf
      weight1 = estimate_radiance(params, omega_in, omega_out, mf_normal, omega_in_dl,
          next_lt, result, direct_light, pdf, pdf_dl);

      Float old_t_max = r_next.t_max;
      r_next = r_sss;
      r_next.t_max = old_t_max;
    } /* if sss */

    const sampled_spectrum color = (next_lt.transport == material::REFLECT) ?
      result.object->surface->refl : result.object->surface->refr;

    // russian roulette path termination
    // only enable when bounce > 3 to reduce noise
    Float rr_prob = 0;
    if (bounce > 3) {
      rr_prob = clamp(1 - color.luminance(), Float(0), params.max_rr);
      if (rng.next_uf() < rr_prob) return sampled_spectrum(0);
    }

    // recursively trace next incident light
    return volume_weight * (result.object->surface->emittance
        + weight0 * weight1 * trace_path(params, r_next, next_lt, sample, rng, bounce + 1))
        / (1 - rr_prob);
  } /* trace_path() */

  void scene::render_routine(
      const render_params& params,
      void (*update_callback)(Float, size_t, size_t))
  {
    job j;
    const size_t n_subpixels = (params.show_depth || params.show_normal) ? 1 : params.n_subpixels;
    size_t sqrt_spp = std::sqrt(params.spp);

    while (master.get_job(&j)) {
      const vector2i start = j.bounds.p_min;
      const vector2i end = j.bounds.p_max;
      for (int y = start.y; y < end.y; ++y) {
        for (int x = start.x; x < end.x; ++x) {

          const int i = params.img_res.x * y + x;

          sampled_spectrum pixel_color(0);
          std::vector<point2f> img_point_offsets;
          std::vector<point2f> bsdf_samples;
          sampler::sample_stratified_2d(
              img_point_offsets,
              n_subpixels,
              std::max(1ul, (size_t) std::sqrt(params.n_subpixels)),
              j.rng
              );

          Float total_weight = 0;
          sampled_spectrum debug_value;

          for (size_t subpixel = 0; subpixel < n_subpixels; ++subpixel) {
            sampled_spectrum color(0.0f);
            const point2f img_point(point2f(x, y) + img_point_offsets[subpixel]);
            const ray r = camera->generate_ray(img_point).normalized();

            sampler::sample_stratified_2d(
                bsdf_samples,
                params.spp,
                std::max(1ul, sqrt_spp),
                j.rng
                );

            for (size_t s = 0; s < params.spp; ++s) {
              color += trace_path(
                  params,
                  r,
                  { material::REFLECT, OUTSIDE },
                  bsdf_samples[s],
                  j.rng,
                  0
                  );
            }

            if (subpixel == 0) debug_value = color;

            point2f pixel_ndc(img_point_offsets[subpixel] * 2 - point2f(1, 1));

            // Use Catmull-Rom parameters for Mitchell filter
            Float weight = mitchell(0.f, 0.5f, pixel_ndc.x) * mitchell(0.f, 0.5f, pixel_ndc.y);
            pixel_color += weight * inv_spp * color;
            total_weight += weight;
          }

          if (params.show_depth || params.show_normal) {
            ird_rgb->at(i) = rgb_spectrum(debug_value[0], debug_value[1], debug_value[2]);
          } else {
            ird_rgb->at(i) = pixel_color.rgb()
              / (COMPARE_EQ(total_weight, 0) ? params.n_subpixels : total_weight);
          }

          // profile if requested
          if (update_callback != nullptr) {
            std::lock_guard<std::mutex> lock(update_mutex);
            ++pixel_counter;

            using namespace std::chrono;
            if ((duration_cast<milliseconds>(system_clock::now() - last_update).count()
                  >= UPDATE_PERIOD) || !rendering())
            {
              size_t elapsed = duration_cast<seconds>(system_clock::now() - render_start).count();
              size_t remaining = params.render_bounds.area() - pixel_counter;
              Float pps = elapsed > 0 ? pixel_counter / elapsed : 0;
              size_t eta = pps ? remaining / pps : 0;
              update_callback(Float(pixel_counter) / params.render_bounds.area(), eta, elapsed);
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
    ird_rgb = std::make_shared<std::vector<rgb_spectrum>>(params.img_res.x * params.img_res.y);
    inv_spp = 1.f / params.spp;
    n_strata = point2i(std::max(1, static_cast<int>(std::sqrt(params.n_subpixels))));

    // init thread scheduler
    master.init(params.render_bounds, params.tile_size);

    last_update   = std::chrono::system_clock::now();
    render_start  = std::chrono::system_clock::now();

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
    using namespace std::chrono;
    size_t elapsed = duration_cast<seconds>(system_clock::now() - render_start).count();
    update_callback(1., 0, elapsed);
    if (profile) {
      profile->time_elapsed = std::chrono::duration_cast<std::chrono::seconds>(
          (std::chrono::system_clock::now() - render_start)
          ).count();
    }

    return ird_rgb;
  } /* render */

  bool scene::rendering() const {
    return ird_rgb->size() > pixel_counter;
  }

  scene::~scene() {
    for (auto it = strand_bvh.begin(); it != strand_bvh.end(); ++it) {
      delete[] it->second;
    }
  }
} /* namespace tracer */
