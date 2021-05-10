#pragma once
#include <glm/glm.hpp>
struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 texcoord;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};
//struct Vertex {
//	// position
//	glm::vec3 Position;
//	// normal
//	glm::vec3 Normal;
//	// texCoords
//	glm::vec2 TexCoords;
//	// tangent
//	glm::vec3 Tangent;
//	// bitangent
//	glm::vec3 Bitangent;
//};