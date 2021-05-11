#pragma once
// standard libs
#include <memory>
#include <unordered_map>
// 3rd libs
#include "Shader.h"
#include<GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Camera.h"
#include <cstdint>
#include "Light.h"
// headers
#include "Model.h"

class RenderEngine
{
public:

#ifdef NDEBUG
	bool is_debug_mode = false;
#else
	bool is_debug_mode = true;
#endif
public:

	RenderEngine(uint32_t _width, uint32_t _height);
	~RenderEngine();
	void Update();
	void Render();
	void Destroy();
	std::shared_ptr<Camera> GetCam() {return main_cam;  }
	uint32_t GetTexture();
private:
	void Initglad();
	void Init();
private:
	uint32_t width{}, height{};
	std::shared_ptr<Camera> main_cam{};
	std::unordered_map<std::string,Shader> shaders{};
	std::unordered_map<std::string, Model> scene;
	uint32_t framebuffer;
	uint32_t textureColorbuffer;

};
