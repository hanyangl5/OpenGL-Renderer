#pragma once
#include <cstdint>
#include <memory>
#include "Framebuffer.h"
#include "Shader.h"
#include "utils.h"
class Aopass
{
public:
    Aopass(uint32_t w, uint32_t h);
    ~Aopass();
    void SetSSAOFactor(float factor);
    void Draw(std::shared_ptr<Framebuffer> color_fbo,
              uint32_t depthtex,
              std::shared_ptr<Quad> quad);
    float factor = 0.05;
private:
    uint32_t width, height;
    std::shared_ptr<Shader> ao_shader;

};
