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

class Mesh {
public:
    // mesh Data
	std::vector<Vertex>       vertices;
	std::vector<uint32_t> indices;
	std::vector<Texture>      textures;
    uint32_t VAO;

    Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Texture> textures);

    void Draw(Shader& shader);

private:
    // render data 
    uint32_t VBO, EBO;

    // initializes all the buffer objects/arrays
    void setupMesh();
};
