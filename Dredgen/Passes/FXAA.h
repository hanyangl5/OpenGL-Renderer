#pragma once

#include <memory>

#include "Render/Framebuffer.h"
#include "Render/Shader.h"
#include "Utils/Utils.h"

class FXAApass {
  public:
    FXAApass(uint32_t w, uint32_t h);
    ~FXAApass();
    void Draw(std::shared_ptr<Framebuffer> color_fbo, std::shared_ptr<Quad> quad);

  public:
    float luma_threshold = 0.6f;

  private:
    std::shared_ptr<Shader> fxaa_shader;
    uint32_t width, height;
};
