
#pragma  once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"
#include "Shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>


uint32_t TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

class Model 
{
public:
    // model data 
    Model() = default;
    Model(std::string const& path, bool gamma = false);
    ~Model();
    void Draw(Shader& shader, RenderMode renermode);
    glm::mat4 GetModelMat();
private:
   void loadModel(std::string const& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
public:
	std::vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::string directory;
	bool gammaCorrection;
	Transform transform;
    RenderMode rendermode = RenderMode::Triangle;
};

