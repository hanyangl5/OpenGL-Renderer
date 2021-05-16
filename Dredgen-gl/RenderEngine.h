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
#include "Skybox.h"
#include "Shadowpass.h"
#include "Uniformbuffer.h"
#include "Framebuffer.h"
#include "Postprocesspass.h"
class RenderEngine
{

public:

	RenderEngine(uint32_t _width, uint32_t _height);
	~RenderEngine();
	void Update();
	void Render();
	//uint32_t RenderAt(std::shared_ptr<Camera> scene_cam);
	void Destroy();
	std::shared_ptr<Camera> GetCam() {return main_cam;  }
	uint32_t GetTexture();
	void AddModel(std::string name,std::string path);
	void GetSceneStat();

private:
	void Initglad();
	void Init();
	//void InitFBO(uint32_t& fbo, uint32_t& ebo);
private:
	uint32_t width{}, height{};

	glm::mat4 projection;
	glm::mat4 view;

	std::shared_ptr<Camera> main_cam{};
	std::shared_ptr<Skybox> skybox{};
	std::unordered_map<std::string,std::shared_ptr<Shader>> shaders{};
	std::unordered_map < std::string, std::shared_ptr<Model>> scene;
	std::vector<std::shared_ptr<Light>> light;

	std::shared_ptr<Shadowpass> shadowpass;
	std::shared_ptr<UboLight> ubolight{};
	std::shared_ptr<Framebuffer> editfbo;
	std::shared_ptr<Framebuffer> renderfbo;
	std::shared_ptr<PostProcesspass> postprocess_pass;

};
