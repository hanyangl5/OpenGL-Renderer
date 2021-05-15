#pragma  once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "Vertex.h"
#include <cstdint>
#include <string>
#include <vector>



struct Texture {
    uint32_t id;
    std::string type;
    std::string path;
};


enum RenderMode
{
	Point, Triangle, Line
};

struct Transform {
	glm::vec3 pos = glm::vec3(0.0); // described as xyz
	glm::vec3 rot = glm::vec3(0.0); // rotate by x/y/z 0-90 
	glm::vec3 scale = glm::vec3(1.0);
};

class Mesh {
public:
    Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Texture> textures);
    void ReleaseBuffer();
    void Draw(Shader& shader, RenderMode renermode);
    glm::mat4 GetModelMat();
private:
    void setupMesh();
public:
    //glm::vec3 translation;
    glm::mat4 model_mat=glm::mat4(1.0);
	std::vector<Vertex>       vertices;
	std::vector<uint32_t> indices;
	std::vector<Texture>      textures;
    uint32_t VAO{};
    std::string meshname;
private:
    uint32_t VBO{}, EBO{};

};
