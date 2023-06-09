#pragma once

#include <memory>
#include <unordered_map>

#include "Render/Shader.h"
#include "Scene/Camera.h"
#include "Scene/Light.h"
#define GLAD_GL_IMPLEMENTATION

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


#include <cstdint>
#include <glm/glm.hpp>

#include "Framebuffer.h"
#include "Passes/SSAO.h"
#include "Passes/Geometry.h"
#include "Passes/FXAA.h"
#include "Passes/PostProcess.h"
#include "Passes/Shadow.h"
#include "Passes/Skybox.h"
#include "Scene/Model.h"
#include "Utils/Utils.h"

enum class RenderBackend {
    OPENGL4,
    DIRECTX11
};

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
    void InitDx11();
    void InitPipelineResources();
    void InitAssets();

  private:
    uint32_t width{}, height{};
    RenderBackend m_backend;
    std::shared_ptr<Camera> main_cam{};
    std::shared_ptr<Skybox> skybox{};
    // std::unordered_map<std::string,std::shared_ptr<Shader>> shaders{};
    std::unordered_map<std::string, std::shared_ptr<Model>> scene;
    std::vector<std::shared_ptr<Light>> lights;

    std::shared_ptr<Framebuffer> base_fbo;
    std::shared_ptr<Shadow> shadowpass;
    std::shared_ptr<PostProcesspass> postprocess_pass;
    std::shared_ptr<GeometyPass> deferred_pass;
    std::shared_ptr<SSAO> ao_pass;
    std::shared_ptr<FXAApass> aa_pass;
    std::shared_ptr<Quad> quad;
};
