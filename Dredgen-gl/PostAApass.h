#pragma once
#include "Framebuffer.h"
#include "Shader.h"
#include "utils.h"
#include <memory>
class PostAApass {
public:
	PostAApass(uint32_t w, uint32_t h);
	~PostAApass();
	void Draw(std::shared_ptr<Framebuffer> color_fbo, std::shared_ptr<Quad> quad);
public:
	float luma_threshold=1.0f;
private:
	std::shared_ptr<Shader> postaa_shader;
	uint32_t width, height;
};
