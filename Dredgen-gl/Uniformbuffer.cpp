#include "Uniformbuffer.h"
#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>
UboLight::UboLight(std::shared_ptr<Shader> shader)
{
	// generate uniform buffer 
	glGenBuffers(1, &ubo_light);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_light);
	glBufferData(GL_UNIFORM_BUFFER, 52, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	uniform_block = glGetUniformBlockIndex(shader->Program, "Light");
	glUniformBlockBinding(shader->Program, uniform_block, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_light);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo_light, 0, 52);
}

UboLight::~UboLight()
{

}

void UboLight::Update(std::shared_ptr<Light> light)
{
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_light);
	int type = 0;
	//std::cout<<sizeof(glm::vec3);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec3), glm::value_ptr(light->GetPos())); //pos
	//std::cout << glm::to_string(light[0]->GetColor());
	glBufferSubData(GL_UNIFORM_BUFFER, 16, sizeof(glm::vec3), glm::value_ptr(light->GetDir()));//dir
	glBufferSubData(GL_UNIFORM_BUFFER, 32, sizeof(glm::vec3), glm::value_ptr(light->GetColor()));// color
	glBufferSubData(GL_UNIFORM_BUFFER, 48, sizeof(int), &type); // type
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
