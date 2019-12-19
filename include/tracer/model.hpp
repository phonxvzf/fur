#ifndef TRACER_MODEL_HPP
#define TRACER_MODEL_HPP

#include <assimp/scene.h>

#include "shape.hpp"

namespace tracer {
  class model {
    private:
      const tf::transform tf_shape_to_world;
      const std::shared_ptr<tracer::material> surface;
      const std::string fpath;

      void load_mesh(
          std::vector<std::shared_ptr<shape>>& shapes,
          aiMesh* mesh
          );

      void load_node(
          std::vector<std::shared_ptr<shape>>& shapes,
          aiNode* node,
          const aiScene* scene
          );

    public:
      model(
          const tf::transform& tf_shape_to_world,
          const std::shared_ptr<tracer::material>& surface,
          const std::string& fpath
          );

      void load(std::vector<std::shared_ptr<shape>>& shapes);
  };
}

#endif /* TRACER_MODEL_HPP */
