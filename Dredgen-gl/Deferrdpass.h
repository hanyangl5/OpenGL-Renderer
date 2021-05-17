#pragma once
#include "Shader.h"
#include <memory>
#include <cstdint>
#include <array>
#include <unordered_map>
#include "Model.h"
#include "Camera.h"
#include "Framebuffer.h"
#include "Skybox.h"
class Deferrdpass
{
public:
	Deferrdpass(uint32_t w,uint32_t h);
	~Deferrdpass();
	void Draw(std::unordered_map < std::string, std::shared_ptr<Model>> scene, std::shared_ptr<Camera> cam, std::shared_ptr<Skybox>skybox = nullptr);
public:
	std::shared_ptr<Framebuffer> fbo;
private:
	uint32_t width, height;
	uint32_t gBuffer,rboDepth;
	uint32_t gPosition, gNormal, gAlbedoSpec;
	std::shared_ptr<Shader> geopass_shader;
	std::shared_ptr<Shader> lightingpass_shader;

	std::array<uint32_t,3> attachments;
	uint32_t quadVAO = 0,quadVBO;

};
