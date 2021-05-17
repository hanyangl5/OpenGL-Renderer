#include "Deferrdpass.h"
#include <glad/glad.h>

const unsigned int NR_LIGHTS = 500;
std::vector<glm::vec3> lightPositions;
std::vector<glm::vec3> lightColors;
//srand(13);

Deferrdpass::Deferrdpass(uint32_t w, uint32_t h) :width(w), height(h)
{
	for (unsigned int i = 0; i < NR_LIGHTS; i++)
	{
		// calculate slightly random offsets
		float xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		float yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
		float zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
		// also calculate random color
		float rColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
		float gColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
		float bColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
		lightColors.push_back(glm::vec3(rColor, gColor, bColor));
	}
	geopass_shader = std::make_shared<Shader>("../resources/shaders/geopassvs.glsl", "../resources/shaders/geopassps.glsl");
	lightingpass_shader = std::make_shared<Shader>("../resources/shaders/lightingpassvs.glsl", "../resources/shaders/lightingpassps.glsl");

	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	// - position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	// - normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	// - color + specular color buffer
	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

	// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 

	attachments = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

	glDrawBuffers(3, attachments.data());

	// generate depth buffer
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	lightingpass_shader->use();
	lightingpass_shader->setInt("gPosition", 0);
	lightingpass_shader->setInt("gNormal", 1);
	lightingpass_shader->setInt("gAlbedoSpec", 2);
}

Deferrdpass::~Deferrdpass()
{
	glDeleteTextures(GL_TEXTURE_2D,&gAlbedoSpec);
	glDeleteTextures(GL_TEXTURE_2D,&gNormal);
	glDeleteTextures(GL_TEXTURE_2D,&gPosition);
	glDeleteBuffers(GL_FRAMEBUFFER, &gBuffer);
	glDeleteRenderbuffers(GL_RENDERBUFFER, &rboDepth);
}

// dst frame buffer, scene, cam, skybox
void Deferrdpass::Draw(std::shared_ptr<Framebuffer>& dst,
	std::unordered_map < std::string, std::shared_ptr<Model>> scene,
	std::shared_ptr<Camera> cam,
	std::shared_ptr<Skybox> skybox)
{
	glViewport(0, 0, width, height);

	// geometry pass
	{
		// configure
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		geopass_shader->use();
		geopass_shader->setMat4("projection", cam->projection);
		geopass_shader->setMat4("view", cam->GetViewMatrix());
		geopass_shader->setVec3("viewPos", cam->Position);
		for (auto& i : scene) {
			i.second->Draw(*geopass_shader.get(), i.second->rendermode);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	// lighting pass
	{

		glBindFramebuffer(GL_FRAMEBUFFER, dst->fbo);
		glViewport(0, 0, width, height);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		lightingpass_shader->use();
		lightingpass_shader->setVec3("viewPos", cam->Position);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);

		// send light relevant uniforms
		for (unsigned int i = 0; i < lightPositions.size(); i++)
		{
			lightingpass_shader->setVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
			lightingpass_shader->setVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
			// update attenuation parameters and calculate radius
			const float linear = 0.7;
			const float quadratic = 1.8;
			lightingpass_shader->setFloat("lights[" + std::to_string(i) + "].Linear", linear);
			lightingpass_shader->setFloat("lights[" + std::to_string(i) + "].Quadratic", quadratic);
		}


		if (quadVAO == 0)
		{
			float quadVertices[] = {
				// positions        // texture Coords
				-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
				 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			};
			// setup plane VAO
			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		}
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glBindVertexArray(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


}
