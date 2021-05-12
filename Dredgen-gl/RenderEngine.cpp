#include <glad/glad.h>
#include "RenderEngine.h"
#include <iostream>
#include <stb_image.h>
#include <glm/gtc/type_ptr.hpp>
#include "Log.h"
#include <array>
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
	//glBindBuffer(GL_UNIFORM_BUFFER, ubo_light);
	//int type = 0;
	//glBufferSubData(GL_UNIFORM_BUFFER, 0*sizeof(glm::vec3), sizeof(glm::vec3), glm::value_ptr(light[0]->GetPos())); //pos
	//glBufferSubData(GL_UNIFORM_BUFFER, 1*sizeof(glm::vec3), sizeof(glm::vec3), glm::value_ptr(light[0]->GetDir()));//dir
	//glBufferSubData(GL_UNIFORM_BUFFER, 2*sizeof(glm::vec3), sizeof(glm::vec3), glm::value_ptr(light[0]->GetColor()));// color
	//glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::vec3)+sizeof(int), sizeof(int), &type); // type
	////glBindBuffer(GL_UNIFORM_BUFFER, 0);
	////
	////glBindBuffer(GL_UNIFORM_BUFFER, ubo_light);

	//glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void RenderEngine::Render()
{
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, edit_fbo);
	glBindTexture(GL_TEXTURE_2D, edit_tbo);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClearColor(0.f, 0.f, 0.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
	auto modelshader = shaders["modelshader"];
	modelshader.use();

	modelshader.setMat4("projection", projection);
	modelshader.setMat4("view", view);

	for (auto object : scene) {
		object.second.Draw(modelshader);
	}

	skybox->Draw(projection, glm::mat3(view));


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
	// clear all relevant buffers
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
	glClear(GL_COLOR_BUFFER_BIT);
}

uint32_t RenderEngine::RenderAt(std::shared_ptr<Camera> scene_cam)
{

	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, render_fbo);
	//glBindTexture(GL_TEXTURE_2D, render_tbo);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClearColor(0.f, 0.f, 0.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
	auto modelshader = shaders["modelshader"];
	modelshader.use();

	modelshader.setMat4("projection", projection);
	modelshader.setMat4("view", scene_cam->GetViewMatrix());

	for (auto object : scene) {
		object.second.Draw(modelshader);
	}

	skybox->Draw(projection, glm::mat3(scene_cam->GetViewMatrix()));


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
	// clear all relevant buffers
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
	glClear(GL_COLOR_BUFFER_BIT);

	return render_fbo;
}

void RenderEngine::Destroy()
{
	for (auto i : scene) {
		i.second.ReleaseBuffer();
	}
}

uint32_t RenderEngine::GetTexture()
{
	return edit_fbo;
}

void RenderEngine::AddModel(std::string name,std::string path)
{
	scene.insert({ name, Model(path) });
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
	light.push_back(std::make_shared<DirectLight>(glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, 2.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));


	InitFBO(edit_fbo, edit_tbo);
	InitFBO(render_fbo, render_tbo);


	shaders.insert({ "modelshader", Shader("../resources/shaders/modelvs.glsl", "../resources/shaders/modelps.glsl","") });
	//scene.insert({ "helmet", {"../resources/models/DamagedHelmet/DamagedHelmet.gltf"} });
	//scene.insert({ "helmet2", {"../resources/models/FlightHelmet/FlightHelmet.gltf"} });
	//scene.insert({ "cerberus", Model("../resources/models/cerberus/scene.gltf") });

	scene["cerberus"].transform.pos = glm::vec3(2.0, 0.0, 0.0);
	scene["cerberus"].transform.scale = glm::vec3(0.03, 0.03, 0.03);

	scene["helmet"].transform.pos = glm::vec3(-3.0, 0.0, 0.0);
	scene["helmet2"].transform.pos = glm::vec3(0.0, 0.0, 0.0);
	scene["helmet2"].transform.scale = glm::vec3(5.0, 5.0, 5.0);
	
	//UI ui

	skybox = std::make_shared<Skybox>("../resources/textures/Indoor");
	//InitUBO();
	;
}

void RenderEngine::InitFBO(uint32_t& fbo, uint32_t& tbo)
{
	glViewport(0, 0, width, height);
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	// create a color attachment texture

	glGenTextures(1, &tbo);
	glBindTexture(GL_TEXTURE_2D, tbo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tbo, 0);
	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Log::Log("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderEngine::InitUBO()
{

	uint32_t uniform_block = glGetUniformBlockIndex(shaders["modelshader"].ID, "Light");
	glUniformBlockBinding(shaders["modelshader"].ID, uniform_block, 0);
	glGenBuffers(GL_UNIFORM_BUFFER, &ubo_light);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_light);
	glBufferData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::vec3) + 4, nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo_light, 0, 3 * sizeof(glm::vec3) + 4);
}

