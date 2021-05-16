#include "Postprocesspass.h"
#include <glad/glad.h>
#include "Log.h"
PostProcesspass::PostProcesspass(uint32_t w, uint32_t h):width(w),height(h)
{
	 postprocess_shader=std::make_shared<Shader>("../resources/shaders/postprocesssvs.glsl", "../resources/shaders/postprocesssps.glsl");

	 fbo = std::make_shared<Framebuffer>(width, height);

	 float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	// positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
	 };

	 glGenVertexArrays(1, &quadVAO);
	 glGenBuffers(1, &quadVBO);
	 glBindVertexArray(quadVAO);
	 glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	 glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	 glEnableVertexAttribArray(0);
	 glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	 glEnableVertexAttribArray(1);
	 glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	 glBindVertexArray(0);

}

PostProcesspass::~PostProcesspass()
{

}

void PostProcesspass::PostProcess(std::shared_ptr<Framebuffer> color_fbo)
{
	glViewport(0, 0, width, height);
	glDisable(GL_DEPTH_TEST); // disable depth test for postprocess pass
	glBindFramebuffer(GL_FRAMEBUFFER, fbo->fbo);
	//glBindRenderbuffer(GL_RENDERBUFFER, fbo->rbo);
	postprocess_shader->use();
	postprocess_shader->setInt("screenTexture", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, color_fbo->tex);
	

	// draw quad
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//// set to default
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDisable(GL_DEPTH_TEST);
	glClearColor(0.f, 0.f, 0.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}
