#include "Postprocesspass.h"
#include <glad/glad.h>
#include "Log.h"

PostProcesspass::PostProcesspass(uint32_t w, uint32_t h):width(w),height(h)
{
	 postprocess_shader=std::make_shared<Shader>("../resources/shaders/postprocesssvs.glsl", "../resources/shaders/postprocesssps.glsl");

	 fbo = std::make_shared<Framebuffer>(width, height);
	 postprocess_shader->use();
	 postprocess_shader->setInt("screenTexture", 0);

}

PostProcesspass::~PostProcesspass()
{

}

void PostProcesspass::Draw(std::shared_ptr<Framebuffer> color_fbo)
{

	glViewport(0, 0, width, height);
	glDisable(GL_DEPTH_TEST); // disable depth test for postprocess pass
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	postprocess_shader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, color_fbo->tex);

	if (quadVAO == 0)
	{
		std::cout << "generate quad vao";
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
	glBindFramebuffer(GL_FRAMEBUFFER, fbo->fbo);
	//glBindRenderbuffer(GL_RENDERBUFFER, fbo->rbo);
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glBindRenderbuffer(GL_RENDERBUFFER, 0);

}
