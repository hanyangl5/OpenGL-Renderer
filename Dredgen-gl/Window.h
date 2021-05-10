#pragma once
// standard libs

// 3rd libs
#include <GLFW/glfw3.h>
// headers
#include "RenderEngine.h"

class Editor {
public:
	Editor(const char* _name, uint32_t _width, uint32_t _height);
	~Editor();
	void Run();
	GLFWwindow* base_window{};
	uint32_t width{}, height{};
private:
	void processInput(GLFWwindow* window);


private:
	std::unique_ptr<RenderEngine> render_engine{};
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
};

class RenderWindow{

};

