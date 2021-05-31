#include <glad/glad.h>
#include "RenderEngine.h"
#include "Log.h"
#include <array>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <stb_image.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <imgui.h>

RenderEngine::RenderEngine(uint32_t _width, uint32_t _height) {
  width = _width;
  height = _height;
  Initglad();
  Init();
}

RenderEngine::~RenderEngine() {}

void RenderEngine::Update() {
  // ubolight->Update(light[0]);
}

void RenderEngine::Render() {

  //shadowpass->Draw(scene, lights);
  deferred_pass->Draw(base_fbo, scene, main_cam,quad, skybox, lights);
  //ao_pass->Draw(base_fbo, deferred_pass->PosTex(),deferred_pass->NormalTex(), quad);
  //postprocess_pass->Draw(base_fbo, quad); // take color buffer as input
  //aa_pass->Draw(base_fbo, quad);
}
uint32_t RenderEngine::GetTexture() { return base_fbo->fbo; }

void RenderEngine::GetSceneStat() {

  // ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize |
  // ImGuiWindowFlags_NoScrollWithMouse;
  ImGui::Begin("scene");
  ImGui::SliderFloat("ssao factor", &ao_pass->factor, 0.001f,0.05f);
  ImGui::SliderFloat("luma threshold", &aa_pass->luma_threshold, 0.00001f,1.0f);
  static int s_selected = -1;
  int index = 0;
  static std::string k;
  {
    ImGui::BeginChild("left pane", ImVec2(300, 0), true);
    // int index = 0;
    for (auto &i : scene) {
      char label[128];
      // sprintf("%s",objects[i].directory.c_str());
      sprintf(label, "%s", i.first.c_str());
      if (ImGui::Selectable(label, s_selected == index)) {
        s_selected = index;
        k = i.first.c_str();
      }
      index++;
    };
    ImGui::EndChild();
  }

  ImGui::SameLine();
  if (s_selected != -1 && !k.empty()) {
    ImGui::BeginGroup();
    ImGui::BeginChild(
        "item view",
        ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1
                                                         // line below us
    ImGui::Text("Object %d", s_selected);
    if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None)) {
      if (ImGui::BeginTabItem("Transform")) {
        auto &chosen_obj = scene.at(k);
        ImGui::InputFloat3("position",
                           glm::value_ptr(chosen_obj->transform.pos));
        ImGui::SliderFloat3(
            "rotation", glm::value_ptr(chosen_obj->transform.rot), 0.0, 360);
        ImGui::SliderFloat3(
            "scale", glm::value_ptr(chosen_obj->transform.scale), 0.01, 5.0);
        // int* a = ;
        // ImGui::InputInt("rendermode",
        // reinterpret_cast<int*>(&chosen_obj.rendermode)); std::cout <<
        // chosen_obj.rendermode<<"\n";
      }
      ImGui::EndTabBar();
    }
    ImGui::EndChild();
    if (ImGui::Button("remove")) {
      // auto iter = scene.find(k);
      // if (iter != scene.end()) {
      //	scene.erase(iter);
      //}
    }
    ImGui::EndGroup();
  }

  ImGui::End();
}
void RenderEngine::Destroy() {}

void RenderEngine::AddModel(std::string name, std::string path) {
  scene.insert({name, std::make_shared<Model>(path)});
}

void RenderEngine::Initglad() {

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    Log::Err("Failed to initialize GLAD");
  } else
    Log::Log("glad inited\n");
}

void RenderEngine::Init() {
  // camera
  main_cam = std::make_shared<Camera>(glm::vec3(0.0f, 0.0f, 2.0f));
  main_cam->SetProjectionMatrix(glm::perspective(glm::radians(main_cam->Zoom), (float)width / (float)height, 0.1f, 1000.0f));


  quad = std::make_shared<Quad>();
  base_fbo = std::make_shared<Framebuffer>(width, height);
  shadowpass = std::make_shared<Shadowpass>(width,height);
  deferred_pass = std::make_shared<Deferrdpass>(width, height);
  ao_pass = std::make_shared<Aopass>(width, height);
  postprocess_pass = std::make_shared<PostProcesspass>(width, height);
  aa_pass = std::make_shared<PostAApass>(width, height);

  // shaders.insert({ "modelshader",
  // std::make_shared<Shader>("../resources/shaders/modelvs.glsl",
  // "../resources/shaders/modelps.glsl") }); ubolight =
  // std::make_shared<UboLight>(shaders.at("modelshader"));

  skybox = std::make_shared<Skybox>("../resources/textures/Indoor",width,height);

  InitAssets();
}

void RenderEngine::InitAssets()
{
	scene.insert({ "helmet",std::make_shared<Model>("../resources/models/DamagedHelmet/DamagedHelmet.gltf") });
    scene["helmet"]->transform.pos = glm::vec3(0.0, 2.0, 0.0);
	scene.insert({ "sponza", std::make_shared<Model>(
								"../resources/models/Sponza/glTF/Sponza.gltf") });

	for (unsigned int i = 0; i < 32; i++) {
	    // calculate slightly random offsets
	    float xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
	    float yPos = ((rand() % 100) / 100.0) * 6.0 ;
	    float zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
	    glm::vec3 pos(xPos, yPos, zPos);
	    // also calculate random color
	    float rColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
	    float gColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
	    float bColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
        glm::vec3 col(rColor, gColor, bColor);
        lights.push_back(std::make_shared<PointLight>(col, pos));
	}
    //lights.push_back(std::make_shared<PointLight>(glm::vec3(1.0, 1.0, 1.0), glm::vec3(0.0, 1.0, 0.0)));
    lights.push_back(std::make_shared<DirectLight>(1.0f * glm::vec3(1.0, 0.9568, 0.4392), glm::vec3(0.0, 0.0, 0.0), glm::vec3(-1.0, -1.0, -1.0)));
  
}
