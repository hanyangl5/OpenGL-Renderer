#include <glad/glad.h>
#include "PostAApass.h"
#include "Log.h"


PostAApass::PostAApass(uint32_t w, uint32_t h) : width(w), height(h) {
	postaa_shader =
		std::make_shared<Shader>("../resources/shaders/postaavs.glsl",
			"../resources/shaders/postaaps.glsl");

	postaa_shader->use();
	postaa_shader->setInt("screenTexture", 0);
}

PostAApass::~PostAApass() {}

void PostAApass::Draw(std::shared_ptr<Framebuffer> color_fbo,
	std::shared_ptr<Quad> quad) {
	glBindFramebuffer(GL_FRAMEBUFFER, color_fbo->fbo);
	glViewport(0, 0, width, height);
	postaa_shader->use();
	postaa_shader->setFloat("luma_threshold", luma_threshold);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, color_fbo->tex);

	quad->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	postaa_shader->unuse();
}
