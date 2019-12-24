#include "tracer/model.hpp"
#include "tracer/shapes/triangle.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <sstream>

namespace tracer {
  model::model(
      const tf::transform& tf_shape_to_world,
      const std::shared_ptr<tracer::material>& surface,
      const std::string& fpath)
    : tf_shape_to_world(tf_shape_to_world), surface(surface), fpath(fpath) {}

  void model::load(std::vector<std::shared_ptr<shape>>& shapes) {
    std::wstringstream wss;
    wss << fpath.c_str();
    std::wcout << L"  * Loading " << wss.str() << L"..." << std::flush;

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(fpath, aiProcess_Triangulate | aiProcess_SortByPType);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
      throw std::runtime_error(std::string("unable to load model: ") + importer.GetErrorString());

    load_node(shapes, scene->mRootNode, scene);

    std::wcout << L" done" << std::endl;
  }

  void model::load_mesh(std::vector<std::shared_ptr<shape>>& shapes, aiMesh* mesh) {
    for (size_t i = 0; i < mesh->mNumFaces; ++i) {
      ASSERT(mesh->mFaces[i].mNumIndices == 3, "mesh->mFaces[i].mNumIndices == 3");
      const int id0 = mesh->mFaces[i].mIndices[0];
      const int id1 = mesh->mFaces[i].mIndices[1];
      const int id2 = mesh->mFaces[i].mIndices[2];
      const point3f a(mesh->mVertices[id0].x, mesh->mVertices[id0].y, mesh->mVertices[id0].z);
      const point3f b(mesh->mVertices[id1].x, mesh->mVertices[id1].y, mesh->mVertices[id1].z);
      const point3f c(mesh->mVertices[id2].x, mesh->mVertices[id2].y, mesh->mVertices[id2].z);

      normal3f normal(0, 0, 0);
      if ((b-a).cross(a-c).is_zero()) continue;
      if (mesh->HasNormals()) {
        normal = (normal3f(mesh->mNormals[id0].x, mesh->mNormals[id0].y, mesh->mNormals[id0].z) 
        + normal3f(mesh->mNormals[id1].x, mesh->mNormals[id1].y, mesh->mNormals[id1].z)
        + normal3f(mesh->mNormals[id2].x, mesh->mNormals[id2].y, mesh->mNormals[id2].z));
        if (!normal.is_zero()) normal = -normal.normalized();
      }
      std::shared_ptr<shape> triangle_mesh(new shapes::triangle(
            tf_shape_to_world,
            surface,
            a, b, c,
            normal
            )
          );
      shapes.push_back(triangle_mesh);
    }
  }

  void model::load_node(
      std::vector<std::shared_ptr<shape>>& shapes,
      aiNode* node,
      const aiScene* scene
      )
  {
    if (node == nullptr) return;
    for (size_t i = 0; i < node->mNumMeshes; ++i) {
      load_mesh(shapes, scene->mMeshes[node->mMeshes[i]]);
    }
    for (size_t i = 0; i < node->mNumChildren; ++i) {
      load_node(shapes, node->mChildren[i], scene);
    }
  }
}
