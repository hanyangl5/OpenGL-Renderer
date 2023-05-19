#pragma once

#include <memory>
#include <unordered_map>

#include "Camera.h"
#include "Light.h"
#include "Shader.h"
#define GLAD_GL_IMPLEMENTATION

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <cstdint>
#include <glm/glm.hpp>

#include "Aopass.h"
#include "Deferrdpass.h"
#include "Framebuffer.h"
#include "Model.h"
#include "Postprocesspass.h"
#include "Shadowpass.h"
#include "Skybox.h"
#include "utils.h"
#include "PostAApass.h"
class RenderEngine {

public:
  RenderEngine(uint32_t _width, uint32_t _height);
  ~RenderEngine();
  void Update();
  void Render();
  void Destroy();
  std::shared_ptr<Camera> GetCam() { return main_cam; }
  uint32_t GetTexture();
  void AddModel(std::string name, std::string path);
  void GetSceneStat();

private:
  void Initglad();
  void Init();
  void InitAssets();
private:
  uint32_t width{}, height{};

  std::shared_ptr<Camera> main_cam{};
  std::shared_ptr<Skybox> skybox{};
  // std::unordered_map<std::string,std::shared_ptr<Shader>> shaders{};
  std::unordered_map<std::string, std::shared_ptr<Model>> scene;
  std::vector<std::shared_ptr<Light>> lights;

  std::shared_ptr<Framebuffer> base_fbo;
  std::shared_ptr<Shadowpass> shadowpass;
  std::shared_ptr<PostProcesspass> postprocess_pass;
  std::shared_ptr<Deferrdpass> deferred_pass;
  std::shared_ptr<Aopass> ao_pass;
  std::shared_ptr<PostAApass> aa_pass;
  std::shared_ptr<Quad> quad;
};
