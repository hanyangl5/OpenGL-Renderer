#include <glad.h>

#include "FXAA.h"
#include "Utils/Log.h"

FXAApass::FXAApass(uint32_t w, uint32_t h) : width(w), height(h) {
    fxaa_shader = std::make_shared<Shader>("../resources/shaders/fxaavs.glsl", "../resources/shaders/fxaaps.glsl");

    fxaa_shader->use();
    fxaa_shader->setInt("screenTexture", 0);
    fxaa_shader->setInt("WIDTH", w);
    fxaa_shader->setInt("HEIGHT", h);
    fxaa_shader->unuse();
    Log::Log("post aa pass initialized, ", w, " ", h);
}

FXAApass::~FXAApass() {}

void FXAApass::Draw(std::shared_ptr<Framebuffer> color_fbo, std::shared_ptr<Quad> quad) {
    glBindFramebuffer(GL_FRAMEBUFFER, color_fbo->fbo);
    glViewport(0, 0, width, height);
    fxaa_shader->use();
    fxaa_shader->setFloat("luma_threshold", luma_threshold);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, color_fbo->tex);

    quad->Draw();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    fxaa_shader->unuse();
}
