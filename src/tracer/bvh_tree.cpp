#include "tracer/bvh_tree.hpp"
#include "tracer/shapes/de_box.hpp"
#include "math/util.hpp"

#define MAX_SHAPES_PER_NODE (4)
#define N_BUCKETS (12)

namespace tracer {
  bvh_tree::bvh_tree(std::vector<std::shared_ptr<shape>> shapes) {
    root = construct_tree(shapes, 0, shapes.size());
  }

  std::shared_ptr<bvh_tree::bvh_node> bvh_tree::construct_tree(
      std::vector<std::shared_ptr<shape>>& shapes,
      int start,
      int end)
  {
    // top-down construction
    if (start >= end) return nullptr;

    std::shared_ptr<bvh_node> node = std::make_shared<bvh_node>();

    // create leaf
    const int n_shapes_node = end - start;
    if (n_shapes_node <= MAX_SHAPES_PER_NODE) {
      // merge bounds
      node->bounds = shapes[start]->world_bounds();
      for (int i = start; i < end; ++i) {
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

      // sort shapes by the selected axis
      std::sort(shapes.begin() + start, shapes.begin() + end,
          [dim](const std::shared_ptr<shape>& a, const std::shared_ptr<shape>& b) -> bool {
            return a->world_bounds().centroid()[dim] < b->world_bounds().centroid()[dim];
          }
          );

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

      const int split = std::max(start + 1, start + best_split * n_shapes_node / N_BUCKETS);
      node->children[0] = construct_tree(shapes, start, split);
      node->children[1] = construct_tree(shapes, split, end);

      // merge children bounds
      if (node->children[0] && node->children[1])
        node->bounds = node->children[0]->bounds.merge(node->children[1]->bounds);
      else if (node->children[0])
        node->bounds = node->children[0]->bounds;
      else if (node->children[1])
        node->bounds = node->children[1]->bounds;
    }

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
      if (inner_hit && (inner_result.t_hit < result->t_hit)) {
        *result = inner_result;
        hit = true;
      }
    }

    if (node->children[0])
      hit |= intersect(node->children[0], r, options, result);
    if (node->children[1])
      hit |= intersect(node->children[1], r, options, result);

    return hit;
  }
} /* namespace tracer */
