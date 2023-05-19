#pragma once
#include "Render/Shader.h"
#include <cstdint>
#include <memory>
#include <string>
#include "Render/Framebuffer.h"
class Skybox {
public:
  Skybox(std::string dirpath, uint32_t w, uint32_t h);
  ~Skybox();
  void Draw(std::shared_ptr<Framebuffer> fbo, glm::mat4 projmat, glm::mat3 viewmat, uint32_t normal_tex);

private:
  uint32_t skybox_texture, skybox_vao, skybox_vbo;
  ;
  std::shared_ptr<Shader> skyboxshader;
  // Shader skyboxshader;
};
