#include "Aopass.h"
#include "Log.h"
#include <glad.h>
#include <glm/glm.hpp>
#include <vector>
Aopass::Aopass(uint32_t w, uint32_t h) : width(w), height(h) {
  ao_shader = std::make_shared<Shader>("../resources/shaders/aovs.glsl",
                                       "../resources/shaders/aops.glsl");
  ao_shader->use();
  ao_shader->setInt("color_texture", 0);
  ao_shader->setInt("pos_tex", 1);
  ao_shader->setInt("normal_tex", 2);
  ao_shader->unuse();
}
void Aopass::SetSSAOFactor(float factor) { this->factor = factor; }
void Aopass::Draw(std::shared_ptr<Framebuffer> color_fbo, uint32_t pos_tex, uint32_t normal_tex,
                  std::shared_ptr<Quad> quad) {

  glBindFramebuffer(GL_FRAMEBUFFER, color_fbo->fbo);
  glViewport(0, 0, width, height);
  ao_shader->use();
  ao_shader->setFloat("ssao_factor", factor);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, color_fbo->tex); // current color buffer
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, pos_tex); // position texture containing depth
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, normal_tex); // normal texture

  quad->Draw();
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  ao_shader->unuse();
}
Aopass::~Aopass() {}
