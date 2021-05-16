#include <glad/glad.h>
#include "RenderEngine.h"
#include <iostream>
#include <stb_image.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "Log.h"
#include <array>

#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

RenderEngine::RenderEngine(uint32_t _width, uint32_t _height)
{
	width = _width;
	height = _height;
	Initglad();
	Init();
}

RenderEngine::~RenderEngine()
{

}

void RenderEngine::Update()
{
	projection = glm::perspective(glm::radians(main_cam->Zoom), (float)width / (float)height, 0.1f, 1000.0f);
	view = main_cam->GetViewMatrix();

	ubolight->Update(light[0]);
}

void RenderEngine::Render()
{
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//shadowpass->Draw(scene);
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, editfbo->fbo);
	glBindRenderbuffer(GL_RENDERBUFFER, editfbo->rbo);
	//glBindTexture(GL_TEXTURE_2D, editfbo->tex);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClearColor(0.f, 0.f, 0.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	auto modelshader = shaders["modelshader"];
	modelshader->use();
	modelshader->setMat4("projection", projection);
	modelshader->setMat4("view", view);

	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, shadowpass->GetShadowTex());
	for (auto object : scene) {
		object.second->Draw(*modelshader.get(), object.second->rendermode);
	}

	//skybox->Draw(projection, glm::mat3(view));

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glClearColor(0.f, 0.f, 0.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	postprocess_pass->PostProcess(editfbo); //take color buffer as input

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDisable(GL_DEPTH_TEST);
	glClearColor(0.f, 0.f, 0.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

//uint32_t RenderEngine::RenderAt(std::shared_ptr<Camera> scene_cam)
//{
//
//	glViewport(0, 0, width, height);
//	glBindFramebuffer(GL_FRAMEBUFFER, renderfbo->fbo);
//	//glBindTexture(GL_TEXTURE_2D, render_tbo);
//	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_CULL_FACE);
//	glCullFace(GL_BACK);
//	glClearColor(0.f, 0.f, 0.f, 1.0f);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	//glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
//	auto modelshader = shaders["modelshader"];
//	modelshader->use();
//	modelshader->setMat4("projection", projection);
//	modelshader->setMat4("view", scene_cam->GetViewMatrix());
//	modelshader->setVec3("eyepos", main_cam->Position);
//	for (auto object : scene) {
//		object.second->Draw(*modelshader.get(), object.second->rendermode);
//	}
//
//	skybox->Draw(projection, glm::mat3(scene_cam->GetViewMatrix()));
//
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	glDisable(GL_DEPTH_TEST);
//	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
//	glClear(GL_COLOR_BUFFER_BIT);
//
//	return renderfbo->fbo;
//}

void RenderEngine::GetSceneStat() {

	//ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollWithMouse;
	ImGui::Begin("scene");
	static int s_selected = -1;
	int index = 0;
	static std::string k;
	{
		ImGui::BeginChild("left pane", ImVec2(300, 0), true);
		//int index = 0;
		for (auto& i : scene) {
			char label[128];
			//sprintf("%s",objects[i].directory.c_str());
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
	if (s_selected != -1 && !k.empty())
	{
		ImGui::BeginGroup();
		ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
		ImGui::Text("Object %d", s_selected);
		if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Transform"))
			{
				auto& chosen_obj = scene.at(k);
				ImGui::InputFloat3("position", glm::value_ptr(chosen_obj->transform.pos));
				ImGui::SliderFloat3("rotation", glm::value_ptr(chosen_obj->transform.rot), 0.0, 360);
				ImGui::SliderFloat3("scale", glm::value_ptr(chosen_obj->transform.scale), 0.01, 5.0);
				//int* a = ;
				//ImGui::InputInt("rendermode", reinterpret_cast<int*>(&chosen_obj.rendermode));
				//std::cout << chosen_obj.rendermode<<"\n";
			}
			ImGui::EndTabBar();
		}
		ImGui::EndChild();
		if (ImGui::Button("remove")) {
			//auto iter = scene.find(k);
			//if (iter != scene.end()) {
			//	scene.erase(iter);
			//}
		}
		ImGui::EndGroup();
	}

	ImGui::End();
}
void RenderEngine::Destroy()
{

}

uint32_t RenderEngine::GetTexture()
{
	//return postprocess_pass->fbo->fbo;
	return editfbo->fbo;
}

void RenderEngine::AddModel(std::string name, std::string path)
{
	scene.insert({ name, std::make_shared<Model>(path) });
}

void RenderEngine::Initglad()
{

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		Log::Err("Failed to initialize GLAD");
	}
	else Log::Log("glad inited\n");
}

void RenderEngine::Init()
{
	// camera
	main_cam = std::make_shared<Camera>(glm::vec3(0.0f, 0.0f, 5.0f));
	light.push_back(std::make_shared<DirectLight>(glm::vec3(1.0, 1.0, 1.0), glm::vec3(0.0, 2.0, 0.0), glm::vec3(0.0, -1.0,-1.0)));
	shadowpass = std::make_shared<Shadowpass>(light[0]);

	editfbo = std::make_shared<Framebuffer>(width, height);
	//renderfbo = std::make_shared<Framebuffer>(width, height);
	shaders.insert({ "modelshader", std::make_shared<Shader>("../resources/shaders/modelvs.glsl", "../resources/shaders/modelps.glsl") });
	ubolight = std::make_shared<UboLight>(shaders.at("modelshader"));

	skybox = std::make_shared<Skybox>("../resources/textures/GraceCathedral");
	//scene.insert({ "plane", std::make_shared<Model>("../resources/models/Cube/glTF/Cube.gltf") });
	scene.insert({ "helmet", std::make_shared<Model>("../resources/models/DamagedHelmet/DamagedHelmet.gltf") });
	scene.insert({ "thorn",std::make_shared<Model>("../resources/models/thorn/thorn.gltf") });
	//scene.insert({ "sponza", std::make_shared<Model>("../resources/models/Sponza/glTF/Sponza.gltf") });
	//scene.at("plane")->transform.scale = glm::vec3(10.0, 0.1, 10.0);
	//scene.at("plane")->transform.pos = glm::vec3(0.0, -2.0, 0.0);

	postprocess_pass = std::make_shared<PostProcesspass>(width,height);
}
