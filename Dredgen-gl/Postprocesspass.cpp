#include "Postprocesspass.h"
#include "Log.h"
#include <glad/glad.h>

PostProcesspass::PostProcesspass(uint32_t w, uint32_t h) : width(w), height(h) {
  postprocess_shader =
      std::make_shared<Shader>("../resources/shaders/postprocesssvs.glsl",
                               "../resources/shaders/postprocesssps.glsl");

  postprocess_shader->use();
  postprocess_shader->setInt("screenTexture", 0);
}

PostProcesspass::~PostProcesspass() {}

void PostProcesspass::Draw(std::shared_ptr<Framebuffer> color_fbo,
                           std::shared_ptr<Quad> quad) {
  glBindFramebuffer(GL_FRAMEBUFFER, color_fbo->fbo);
  glViewport(0, 0, width, height);
  postprocess_shader->use();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, color_fbo->tex);

  quad->Draw();

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
