#include <glad/glad.h>
#include "RenderEngine.h"
#include <iostream>
#include "Log.h"

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

}

void RenderEngine::Render()
{

	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.f, 0.f, 0.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	
	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
	auto modelshader = shaders["modelshader"];
	modelshader.use();
	glm::mat4 projection = glm::perspective(glm::radians(main_cam->Zoom), (float)width / (float)height, 0.1f, 1000.0f);
	glm::mat4 view = main_cam->GetViewMatrix();
	modelshader.setMat4("projection", projection);
	modelshader.setMat4("view", view);

	for (auto object : scene) {
		object.second.Draw(modelshader);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
	// clear all relevant buffers
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
	glClear(GL_COLOR_BUFFER_BIT);
}

void RenderEngine::Destroy()
{

}

uint32_t RenderEngine::GetTexture()
{
	return framebuffer;
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
	
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, width, height);

	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// create a color attachment texture

	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
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


	shaders.insert({ "modelshader", Shader("../resources/shaders/modelvs.glsl", "../resources/shaders/modelps.glsl","") });
	scene.insert({ "helmet", Model("../resources/models/DamagedHelmet/DamagedHelmet.gltf") });
	scene.insert({ "helmet2", Model("../resources/models/FlightHelmet/FlightHelmet.gltf") });
	scene.insert({ "cerberus", Model("../resources/models/cerberus/scene.gltf") });
	
	scene["cerberus"].transform.pos = glm::vec3(2.0, 0.0, 0.0);
	scene["cerberus"].transform.scale = glm::vec3(0.03, 0.03, 0.03);

	scene["helmet"].transform.pos = glm::vec3(-3.0, 0.0, 0.0);
	scene["helmet2"].transform.pos = glm::vec3(0.0, 0.0, 0.0);
	scene["helmet2"].transform.scale = glm::vec3(5.0, 5.0, 5.0);

	//UI ui;
}
