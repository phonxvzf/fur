#include "tracer/bvh_tree.hpp"
#include "tracer/shapes/de_box.hpp"
#include "math/util.hpp"

#define MAX_SHAPES_PER_NODE (2)

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
      int mid = (start + end) >> 1;
      node->children[0] = construct_tree(shapes, start, mid);
      node->children[1] = construct_tree(shapes, mid, end);

      // merge children bounds
      int exist = 0;
      if (node->children[0]) {
        node->bounds = node->children[0]->bounds;
        exist = 0;
      } else if (node->children[1]) {
        node->bounds = node->children[1]->bounds;
        exist = 1;
      }
      node->bounds = node->children[exist ^ 1] ?
        node->children[exist ^ 1]->bounds.merge(node->bounds) : node->bounds;
    }

    return node;
  }

  bool bvh_tree::intersect_bounds(
      const ray& r,
      const bounds3f& bounds,
      const shape::intersect_opts& options
      ) const
  {
    vector3f b = 0.5f * bounds.diagonal();
    shapes::de_box box(tf::translate(bounds.p_min + b), nullptr, b);
    return box.intersect(r, options, OUTSIDE, nullptr);
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
      const shape* ignored_shape,
      material::medium med,
      shape::intersect_result* result
      ) const
  {
    return intersect(root, r, options, ignored_shape, med, result);
  }

  bool bvh_tree::intersect(
      const std::shared_ptr<bvh_node>& node,
      const ray& r,
      const shape::intersect_opts& options,
      const shape* ignored_shape,
      material::medium med,
      shape::intersect_result* result
      ) const
  {
    if (!intersect_bounds(r, node->bounds, options)) return false;

    bool hit = false;
    if (node->shapes.size() > 0) {
      for (size_t i = 0; i < node->shapes.size(); ++i) {
        if (node->shapes[i].get() != ignored_shape) {
          shape::intersect_result inner_result;
          hit = node->shapes[i]->intersect(r, options, med, &inner_result);
          if (hit && (inner_result.t_hit < result->t_hit)) *result = inner_result;
        }
      }
    }

    if (node->children[0])
      hit |= intersect(node->children[0], r, options, ignored_shape, med, result);
    if (node->children[1])
      hit |= intersect(node->children[1], r, options, ignored_shape, med, result);

    return hit;
  }
} /* namespace tracer */
