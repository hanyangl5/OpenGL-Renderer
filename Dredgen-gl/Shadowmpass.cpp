#include "Shadowpass.h"
#include <glad/glad.h>
#include "Log.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
Shadowpass::Shadowpass(std::shared_ptr<Light> light)
{
	glGenFramebuffers(1, &shadow_fbo);
	glGenTextures(1, &shadow_tex);
	glBindTexture(GL_TEXTURE_2D, shadow_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, shadow_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow_tex, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	shadowshader = std::make_shared<Shader>("../resources/shaders/shadowvs.glsl", "../resources/shaders/shadowps.glsl");
}

Shadowpass::~Shadowpass()
{

}

void Shadowpass::Draw(std::unordered_map < std::string, std::shared_ptr<Model>>& scene) {

	glClear(GL_DEPTH_BUFFER_BIT);

	float near_plane = 1.0f, far_plane = 7.5f;
	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;


	shadowshader->use();
	shadowshader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, shadow_fbo);

	glClear(GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, woodTexture);

	for (auto& i : scene)
	{
		i.second->Draw(*shadowshader, RenderMode::Triangle);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


}


uint32_t Shadowpass::GetShadowTex() const
{
	return shadow_tex;
}
