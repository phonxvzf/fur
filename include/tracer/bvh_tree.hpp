#ifndef TRACER_BVH_TREE_HPP
#define TRACER_BVH_TREE_HPP

#include "shape.hpp"
#include <thread>
#include <mutex>
#include <condition_variable>

namespace tracer {
  class bvh_tree {
    private:
      struct bvh_node {
        bounds3f bounds;
        std::shared_ptr<bvh_node> children[2] = { nullptr, nullptr };
        std::vector<std::shared_ptr<shape>> shapes;
      };

      int n_available_workers;

      std::shared_ptr<bvh_node> root;

      std::shared_ptr<bvh_node> construct_tree(
          std::vector<std::shared_ptr<shape>>& shapes,
          int start,
          int end,
          std::shared_ptr<bvh_node>* ret_node = nullptr
          );

      size_t n_shapes(const std::shared_ptr<bvh_node>& node) const;

      bool intersect(
          const std::shared_ptr<bvh_node>& node,
          const ray& r,
          const shape::intersect_opts& options,
          shape::intersect_result* result
          ) const;

      std::thread* dispatch_construction(
          std::vector<std::shared_ptr<shape>>& shapes,
          int start,
          int end,
          std::shared_ptr<bvh_node>* ret_node = nullptr
          );

      void wait_worker(std::thread* worker);

      bool workers_available() const;

    public:
      bvh_tree(std::vector<std::shared_ptr<shape>> shapes);

      size_t n_shapes() const;

      bool intersect(
          const ray& r,
          const shape::intersect_opts& options,
          shape::intersect_result* result
          ) const;
  };
}

#endif /* TRACER_BVH_TREE_HPP */
