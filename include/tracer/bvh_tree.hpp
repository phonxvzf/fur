#ifndef TRACER_BVH_TREE_HPP
#define TRACER_BVH_TREE_HPP

#include "shape.hpp"

namespace tracer {
  class bvh_tree {
    private:

      struct bvh_node {
        bounds3f bounds;
        bvh_node* children[2];
        std::shared_ptr<shape> shapes[2];
      };

    public:
      bvh_tree(const std::vector<std::shared_ptr<shape>>& shapes);
  };
}

#endif /* TRACER_BVH_TREE_HPP */
