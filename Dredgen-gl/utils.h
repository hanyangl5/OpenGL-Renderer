#pragma once
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
class Quad {
public:
  Quad();
  ~Quad();
  void Draw();
  uint32_t quad_vao, quad_vbo;
};

inline glm::mat4 PerspectiveFovReverseZLH_ZO(float fov, float width, float height, float zNear) {
	const float h = 1.0f/glm::tan(0.5f * fov);
	const float w = h * height / width;
	glm::mat4 result = glm::zero<glm::mat4>();
	result[0][0] = w;
	result[1][1] = h;
	result[2][2] = 0.0f;
	result[2][3] = 1.0f;
	result[3][2] = zNear;
	return result;
};

inline glm::mat4 MakeInfReversedZProjRH(float fovY_radians, float aspectWbyH, float zNear)
{
	float f = 1.0f / tan(fovY_radians / 2.0f);
	return glm::mat4(
		f / aspectWbyH, 0.0f, 0.0f, 0.0f,
		0.0f, f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, zNear, 0.0f);
}