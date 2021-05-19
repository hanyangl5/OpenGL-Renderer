#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "Log.h"
#include "Model.h"
#include <assimp/pbrmaterial.h>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <stb_image.h>

Model::Model(std::string const &path, bool gamma) {
  gammaCorrection = gamma;
  loadModel(path);
}

Model::~Model() {
  for (auto &i : meshes) {
    i->ReleaseBuffer();
  }
}

// draws the model, and thus all its meshes
void Model::Draw(Shader &shader, RenderMode renermode = Triangle) {

  for (uint32_t i = 0; i < meshes.size(); i++) {
    glm::mat4 model_mat = GetModelMat() * meshes[i]->GetModelMat();
    shader.setMat4("model", model_mat);
    meshes[i]->Draw(shader, rendermode);
  }
}

glm::mat4 Model::GetModelMat() {
  glm::mat4 ret = glm::mat4(1.0);
  ret = glm::translate(ret, transform.pos);
  //
  if (1) {
    glm::quat myQuat(glm::radians(transform.rot));
    glm::mat4 RotationMatrix = glm::toMat4(myQuat);
    ret = RotationMatrix * ret;
  } else {
    // rotate by z y x
    ret = glm::rotate(ret, glm::radians(transform.rot[1]), glm::vec3(0, 1, 0));
    ret = glm::rotate(ret, glm::radians(transform.rot[2]), glm::vec3(0, 0, 1));
    ret = glm::rotate(ret, glm::radians(transform.rot[0]), glm::vec3(1, 0, 0));
  }

  ret = glm::scale(ret, transform.scale);

  return ret;
}

void Model::loadModel(std::string const &path) {
  Log::Log("loading model at:", path);
  Assimp::Importer importer;
  const aiScene *scene = importer.ReadFile(
      path, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    Log::Log("ERROR::ASSIMP:: ", importer.GetErrorString());
    return;
  }
  directory = path.substr(0, path.find_last_of('/'));
  processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
  for (uint32_t i = 0; i < node->mNumMeshes; i++) {
    node->mTransformation;
    aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    Mesh _mesh = processMesh(mesh, scene);
    _mesh.meshname = node->mName.C_Str();
    auto t = node->mTransformation;
    _mesh.model_mat = glm::mat4(t.a1, t.b1, t.c1, t.d1, t.a2, t.b2, t.c2, t.d2,
                                t.a3, t.b3, t.c3, t.d3, t.a4, t.b4, t.c4, t.d4);
    meshes.push_back(std::make_shared<Mesh>(_mesh));
  }
  for (uint32_t i = 0; i < node->mNumChildren; i++) {
    processNode(node->mChildren[i], scene);
  }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
  // data to fill
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
  std::vector<Texture> textures;

  // walk through each of the mesh's vertices
  for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex;
    glm::vec3 vector; // we declare a placeholder vector since assimp uses its
                      // own vector class that doesn't directly convert to glm's
                      // vec3 class so we transfer the data to this placeholder
                      // glm::vec3 first.
    // positions
    vector.x = mesh->mVertices[i].x;
    vector.y = mesh->mVertices[i].y;
    vector.z = mesh->mVertices[i].z;
    vertex.pos = vector;
    // normals
    if (mesh->HasNormals()) {
      vector.x = mesh->mNormals[i].x;
      vector.y = mesh->mNormals[i].y;
      vector.z = mesh->mNormals[i].z;
      vertex.normal = vector;
    }
    // texture coordinates
    if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
    {
      glm::vec2 vec;
      // a vertex can contain up to 8 different texture coordinates. We thus
      // make the assumption that we won't use models where a vertex can have
      // multiple texture coordinates so we always take the first set (0).
      vec.x = mesh->mTextureCoords[0][i].x;
      vec.y = mesh->mTextureCoords[0][i].y;
      vertex.texcoord = vec;
      // tangent
      vector.x = mesh->mTangents[i].x;
      vector.y = mesh->mTangents[i].y;
      vector.z = mesh->mTangents[i].z;
      vertex.tangent = vector;
      // bitangent
      vector.x = mesh->mBitangents[i].x;
      vector.y = mesh->mBitangents[i].y;
      vector.z = mesh->mBitangents[i].z;
      vertex.bitangent = vector;
    } else
      vertex.texcoord = glm::vec2(0.0f, 0.0f);

    vertices.push_back(vertex);
  }
  // now wak through each of the mesh's faces (a face is a mesh its triangle)
  // and retrieve the corresponding vertex indices.
  for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];
    // retrieve all indices of the face and store them in the indices vector
    for (uint32_t j = 0; j < face.mNumIndices; j++)
      indices.push_back(face.mIndices[j]);
  }
  // process materials
  aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
  // we assume a convention for sampler names in the shaders. Each diffuse
  // texture should be named as 'texture_diffuseN' where N is a sequential
  // number ranging from 1 to MAX_SAMPLER_NUMBER. Same applies to other texture
  // as the following list summarizes: diffuse: texture_diffuseN specular:
  // texture_specularN normal: texture_normalN

  // 1. albedo maps

  std::vector<Texture> albedoMaps =
      loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_albedo");
  textures.insert(textures.end(), albedoMaps.begin(), albedoMaps.end());

  // 3. normal maps
  std::vector<Texture> normalMaps =
      loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
  textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

  std::vector<Texture> MetallicRoughnessMaps = loadMaterialTextures(
      material, aiTextureType_UNKNOWN, "texture_metallicroughness");
  textures.insert(textures.end(), MetallicRoughnessMaps.begin(),
      MetallicRoughnessMaps.end());

  Log::Log("albedo texture:");
  for (auto& i : albedoMaps) {
	  Log::Log(i.path);
  }
  Log::Log("metallic texture:");
  for (auto &i : MetallicRoughnessMaps) {
     Log::Log(i.path);
  }

  // return a mesh object created from the extracted mesh data
  return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat,
                                                 aiTextureType type,
                                                 std::string typeName) {
  std::vector<Texture> textures;
  for (uint32_t i = 0; i < mat->GetTextureCount(type); i++) {
    aiString str;
    mat->GetTexture(type, i, &str);
    // check if texture was loaded before and if so, continue to next iteration:
    // skip loading a new texture
    bool skip = false;
    for (uint32_t j = 0; j < textures_loaded.size(); j++) {
      if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
        textures.push_back(textures_loaded[j]);
        skip = true; // a texture with the same filepath has already been
                     // loaded, continue to next one. (optimization)
        break;
      }
    }
    if (!skip) { // if texture hasn't been loaded already, load it
      Texture texture;
      texture.id = TextureFromFile(str.C_Str(), this->directory);
      texture.type = typeName;
      texture.path = str.C_Str();
      textures.push_back(texture);
      textures_loaded.push_back(
          texture); // store it as texture loaded for entire model, to ensure we
                    // won't unnecesery load duplicate textures.
    }
  }
  return textures;
}

uint32_t TextureFromFile(const char *path, const std::string &directory,
                         bool gamma) {
  std::string dir = directory.substr(0, directory.find_last_of('\\'));
  std::string filename = std::string(path);
  filename = dir + '/' + filename;
  // Log::Log(filename);
  uint32_t textureID;
  glGenTextures(1, &textureID);

  int width, height, nrComponents;
  unsigned char *data =
      stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
  if (data) {
    GLenum format;
    if (nrComponents == 1)
      format = GL_RED;
    else if (nrComponents == 3)
      format = GL_RGB;
    else if (nrComponents == 4)
      format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  } else {
    std::cout << "Texture failed to load at path: " << path << std::endl;
    stbi_image_free(data);
  }

  return textureID;
}
