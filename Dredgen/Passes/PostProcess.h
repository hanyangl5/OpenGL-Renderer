#pragma once
#include "Render/Framebuffer.h"
#include "Render/Shader.h"
#include "Utils/Utils.h"
#include <memory>
class PostProcesspass {
  public:
    PostProcesspass(uint32_t w, uint32_t h);
    ~PostProcesspass();
    void Draw(std::shared_ptr<Framebuffer> color_fbo, std::shared_ptr<Quad> quad);

  private:
    std::shared_ptr<Shader> postprocess_shader;
    uint32_t width, height;
};
