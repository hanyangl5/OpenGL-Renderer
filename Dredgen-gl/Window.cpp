#include <glad/glad.h>
#include <GLFW/glfw3.h>
// headers
#include "Window.h"
#include "Log.h"
float lastX = 1280 / 2.0f;
float lastY = 800 / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float xoffset=0.0f, yoffset=0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	xoffset = xpos - lastX;
	yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;
}

void button_callback(GLFWwindow* window, int a, int b, int c) {

}

Editor::Editor(const char* _name, uint32_t _width, uint32_t _height) : width(_width), height(_height)
{
	if (glfwInit() != GLFW_TRUE)
	{
		glfwTerminate();
		Log::Log("failed to init glfw");
	};

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	base_window = glfwCreateWindow(1280, 800, "Dredgen-gl", nullptr, nullptr);

	if (!base_window)
	{
		glfwTerminate();
		Log::Err("failed to init window");
	}


	glfwMakeContextCurrent(base_window);

	glfwSetFramebufferSizeCallback(base_window,framebuffer_size_callback);
	glfwSetCursorPosCallback(base_window, mouse_callback);
	glfwSetMouseButtonCallback(base_window, button_callback);
	glfwSetInputMode(base_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}
Editor::~Editor()
{
	glfwDestroyWindow(base_window);
	glfwTerminate();
	Log::Log("destroyed");
}

void Editor::Run()
{
	render_engine = std::make_unique<RenderEngine>(width, height, base_window);

	while (!glfwWindowShouldClose(base_window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(base_window);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		render_engine->Update();
		render_engine->Render();

		glfwSwapBuffers(base_window);
		glfwPollEvents();

	}
	render_engine->Destroy();

}


void Editor::processInput(GLFWwindow* window)
{
	// drag with right mouse button 
	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2))
		render_engine->GetCam()->ProcessMouseMovement(xoffset, yoffset);

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		render_engine->GetCam()->ProcessKeyboard(Direction::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		render_engine->GetCam()->ProcessKeyboard(Direction::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		render_engine->GetCam()->ProcessKeyboard(Direction::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		render_engine->GetCam()->ProcessKeyboard(Direction::RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		render_engine->GetCam()->ProcessKeyboard(Direction::UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		render_engine->GetCam()->ProcessKeyboard(Direction::DOWN, deltaTime);

	//std::cout << deltaTime<<render_engine->GetCam()->Position.x << " " << render_engine->GetCam()->Position.y << " " << render_engine->GetCam()->Position.z << "\n";
}
