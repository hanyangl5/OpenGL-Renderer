#pragma once
// standard libs

// 3rd libs
#include <GLFW/glfw3.h>
// headers
#include "RenderEngine.h"

class Logger {};

class Inspector {};

class EditMode {
public:
	EditMode(uint32_t w, uint32_t h):width(w),height(h) {
		render_engine = std::make_unique<RenderEngine>(w, h);
	}
	uint32_t GetTexture();
	void Render();
	uint32_t width, height;
	std::unique_ptr<RenderEngine> render_engine{};

};


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
	std::shared_ptr<EditMode> edit_mode;
	float lastX=0.0f, lastY=0.0f;
	float xoffset = 0.0f, yoffset = 0.0f;
	bool firstMouse = true;
};


