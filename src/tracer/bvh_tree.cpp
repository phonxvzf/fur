#include "tracer/bvh_tree.hpp"
#include "tracer/shapes/de_box.hpp"
#include "math/util.hpp"

#define MAX_SHAPES_PER_NODE (4)
#define N_BUCKETS (16)

namespace tracer {
  bvh_tree::bvh_tree(std::vector<std::shared_ptr<shape>> shapes) {
    n_available_workers = std::thread::hardware_concurrency();
    root = construct_tree(shapes, 0, shapes.size());
  }

  std::shared_ptr<bvh_tree::bvh_node> bvh_tree::construct_tree(
      std::vector<std::shared_ptr<shape>>& shapes,
      int start,
      int end,
      std::shared_ptr<bvh_node>* ret_node
      )
  {
    // top-down construction
    if (start >= end) return nullptr;

    std::shared_ptr<bvh_node> node = std::make_shared<bvh_node>();

    // create leaf
    const int n_shapes_node = end - start;
    if (n_shapes_node <= MAX_SHAPES_PER_NODE) {
      // merge bounds
      node->bounds = shapes[start]->world_bounds();
      for (int i = start + 1; i < end; ++i) {
        node->bounds = node->bounds.merge(shapes[i]->world_bounds());
      }
      for (int i = 0; i < n_shapes_node; ++i) {
        node->shapes.push_back(shapes[start + i]);
      }
    } else {
      // select partition dimension by determining which centroid bounds axis is the longest
      bounds3f centroid_bounds = shapes[start]->world_bounds().centroid();
      for (int i = start; i < end; ++i) {
        centroid_bounds = centroid_bounds.merge(shapes[i]->world_bounds().centroid());
      }

      int dim = centroid_bounds.which_longest();
      node->split_dim = dim;

      // partition shapes
      Float cost[N_BUCKETS];
      for (int i = 0; i < N_BUCKETS; ++i) {
        const int n_left = i * n_shapes_node / N_BUCKETS;
        const int n_right = n_shapes_node - n_left;
        bounds3f bounds_left = shapes[start]->world_bounds();
        bounds3f bounds_right = shapes[start + n_left]->world_bounds();
        for (int j = 0; j < n_left; ++j)
          bounds_left = bounds_left.merge(shapes[start + j]->world_bounds());
        for (int j = 0; j < n_right; ++j)
          bounds_right = bounds_right.merge(shapes[start + n_left + j]->world_bounds());
        bounds3f total_bounds = bounds_left.merge(bounds_right);
        cost[i] = 0.125f
          + (bounds_left.surface_area() * n_left + bounds_right.surface_area() * n_right)
          / total_bounds.surface_area();
      }

      int best_split = 0;
      Float best_cost = cost[0];
      for (int i = 1; i < N_BUCKETS; ++i) {
        if (cost[i] < best_cost) {
          best_cost   = cost[i];
          best_split  = i;
        }
      }

      // partiion shapes by the selected axis
      auto pivot = std::partition(shapes.begin() + start, shapes.begin() + end,
          [&](const std::shared_ptr<shape>& s) -> bool {
            // find bucket position
            Float bucket_i = centroid_bounds.uvw(
                s->world_bounds().centroid()
                )[dim] * N_BUCKETS;
            return bucket_i < best_split;
          }
          );

      int split = pivot - shapes.begin();
      if (split <= start) split = std::max(start + 1, start + std::rand() % n_shapes_node);

      std::thread *worker0, *worker1;
      worker0 = dispatch_construction(shapes, start, split, &node->children[0]);
      worker1 = dispatch_construction(shapes, split, end, &node->children[1]);
      wait_worker(worker0);
      wait_worker(worker1);

      // merge children bounds
      if (node->children[0] && node->children[1])
        node->bounds = node->children[0]->bounds.merge(node->children[1]->bounds);
      else if (node->children[0])
        node->bounds = node->children[0]->bounds;
      else if (node->children[1])
        node->bounds = node->children[1]->bounds;
    }

    if (ret_node) *ret_node = node;
    return node;
  }

  size_t bvh_tree::n_shapes(const std::shared_ptr<bvh_node>& node) const {
    if (node == nullptr) return 0;
    return node->shapes.size() + n_shapes(node->children[0]) + n_shapes(node->children[1]);
  }

  size_t bvh_tree::n_shapes() const {
    return n_shapes(root);
  }

  bool bvh_tree::intersect(
      const ray& r,
      const shape::intersect_opts& options,
      shape::intersect_result* result
      ) const
  {
    return intersect(root, r, options, result);
  }

  bool bvh_tree::intersect(
      const std::shared_ptr<bvh_node>& node,
      const ray& r,
      const shape::intersect_opts& options,
      shape::intersect_result* result
      ) const
  {
    if (!node->bounds.intersect(r)) return false;

    bool hit = false;
    for (size_t i = 0; i < node->shapes.size(); ++i) {
      shape::intersect_result inner_result;
      bool inner_hit = node->shapes[i]->intersect(r, options, &inner_result);
      if (inner_hit) {
        hit = true;
        if (inner_result.t_hit < result->t_hit) {
          *result = inner_result;
        }
      }
    }

    int left = 0, right = 1;
    point3i negative_dir(r.dir.x < 0, r.dir.y < 0, r.dir.z < 0);
    if (node->split_dim >= 0) {
      if (negative_dir[node->split_dim]) std::swap(left, right);
      if (node->children[left])
        hit |= intersect(node->children[left], r, options, result);
      if (node->children[right])
        hit |= intersect(node->children[right], r, options, result);
    }

    return hit;
  }

  bool bvh_tree::occluded(const ray& r, const shape::intersect_opts& options) const {
    return occluded(root, r, options);
  }

  bool bvh_tree::occluded(
      const std::shared_ptr<bvh_node>& node,
      const ray& r, 
      const shape::intersect_opts& options
      ) const
  {
    if (!node->bounds.intersect(r)) return false;

    shape::intersect_result inner_result;
    for (size_t i = 0; i < node->shapes.size(); ++i) {
      if (node->shapes[i]->intersect(r, options, &inner_result)) return true;
    }

    int left = 0, right = 1;
    point3i negative_dir(r.dir.x < 0, r.dir.y < 0, r.dir.z < 0);
    if (node->split_dim >= 0) {
      if (negative_dir[node->split_dim]) std::swap(left, right);
      if (node->children[left])
        if (occluded(node->children[left], r, options))
          return true;
      if (node->children[right])
        if (occluded(node->children[right], r, options))
          return true;
    }

    return false;
  }

  std::thread* bvh_tree::dispatch_construction(
      std::vector<std::shared_ptr<shape>>& shapes,
      int start,
      int end,
      std::shared_ptr<bvh_node>* ret_node
      )
  {
    if (n_available_workers == 0) {
      construct_tree(shapes, start, end, ret_node);
      return nullptr;
    }

    --n_available_workers;

    std::thread *worker = new std::thread(
        &bvh_tree::construct_tree, this, std::ref(shapes), start, end, ret_node
        );

    return worker;
  }

  void bvh_tree::wait_worker(std::thread* worker) {
    if (worker == nullptr) return;
    worker->join();
    ++n_available_workers;
    delete worker;
  }

  bool bvh_tree::workers_available() const {
    return n_available_workers > 0;
  }
} /* namespace tracer */
