#pragma  once
#include <cstdint>
class Framebuffer
{
public:
	Framebuffer(uint32_t w,uint32_t h);
	~Framebuffer();

//private:
	uint32_t fbo, tex, rbo;
	uint32_t width, height;
};
