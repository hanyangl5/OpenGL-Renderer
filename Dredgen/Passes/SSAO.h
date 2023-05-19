#pragma once

#include <cstdint>
#include <memory>

#include "Utils/Utils.h"

#include "Render/Framebuffer.h"
#include "Render/Shader.h"

class SSAO {
  public:
    SSAO(uint32_t w, uint32_t h);
    ~SSAO();
    void SetSSAOFactor(float factor);
    void Draw(std::shared_ptr<Framebuffer> color_fbo, uint32_t depthtex, uint32_t normal_tex,
              std::shared_ptr<Quad> quad);

    float factor = 0.001f;

  private:
    uint32_t width, height;
    std::shared_ptr<Shader> ao_shader;
};
