
#pragma once
#include "Mesh.h"
#include "Shader.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

uint32_t TextureFromFile(const char *path, const std::string &directory,
                         bool gamma = false);

class Model {
public:
  // model data
  Model() = default;
  Model(std::string const &path, bool gamma = false);
  ~Model();
  void Draw(Shader &shader, RenderMode renermode);
  glm::mat4 GetModelMatrix();

private:
  void LoadModel(std::string const &path);
  void ProcessNode(aiNode *node, const aiScene *scene);
  Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
  std::vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                            std::string typeName);

public:
  std::vector<Texture>
      textures_loaded; // stores all the textures loaded so far, optimization to
                       // make sure textures aren't loaded more than once.
  std::vector<std::shared_ptr<Mesh>> meshes;
  std::string directory;
  bool gammaCorrection;
  Transform transform;
  RenderMode rendermode = RenderMode::Triangle;
};
