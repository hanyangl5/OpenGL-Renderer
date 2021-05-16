#pragma  once
#include "Framebuffer.h"
#include <memory>
#include "Shader.h"
class PostProcesspass
{
public:
	PostProcesspass(uint32_t w,uint32_t h);
	~PostProcesspass();
	void PostProcess(std::shared_ptr<Framebuffer> color_fbo);

//private:
	std::shared_ptr<Framebuffer> fbo;
	std::shared_ptr<Shader> postprocess_shader;
	uint32_t width, height;
	uint32_t quadVAO, quadVBO;
};
