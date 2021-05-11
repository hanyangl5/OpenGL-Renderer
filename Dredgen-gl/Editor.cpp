#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
// headers
#include "Editor.h"
#include "Log.h"


Editor::Editor(const char* _name, uint32_t _width, uint32_t _height) : width(_width), height(_height)
{
	width = _width;
	height = _height;
	if (glfwInit() != GLFW_TRUE)
	{
		glfwTerminate();
		Log::Log("failed to init glfw");
	};

	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	base_window = glfwCreateWindow(width, height, "Dredgen-gl", nullptr, nullptr);

	if (!base_window)
	{
		glfwTerminate();
		Log::Err("failed to init window");
	}


	glfwMakeContextCurrent(base_window);
	glfwSwapInterval(1); // Enable vsync

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		Log::Err("Failed to initialize GLAD");
	}
	edit_mode = std::make_shared<EditMode>(800, 600);
}
Editor::~Editor()
{
	glfwDestroyWindow(base_window);
	glfwTerminate();
	Log::Log("destroyed");
}

void Editor::Run()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	
	ImGui::StyleColorsLight();
	io.Fonts->AddFontFromFileTTF("../resources/fonts/Consolas.ttf", 17);
	const char* glsl_version = "#version 130";
	ImGui_ImplGlfw_InitForOpenGL(base_window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	ImVec4 clear_color = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);
	bool show_demo_window = true;
	bool show_another_window = false;
	// Main loop
	while (!glfwWindowShouldClose(base_window))
	{
		glfwPollEvents();

		// process user input
		{
			float xpos = io.MousePos.x;
			float ypos = io.MousePos.y;
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
			processInput(base_window);
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		{
			if (ImGui::BeginMainMenuBar())
			{
				if (ImGui::BeginMenu("Import Asset"))
				{
					if (ImGui::MenuItem("Import from file")) {
						system("explorer C:\\");
					}
					
					ImGui::EndMenu();
				}
				//if (ImGui::BeginMenu("Edit"))
				//{
				//	if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
				//	if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
				//	ImGui::Separator();
				//	if (ImGui::MenuItem("Cut", "CTRL+X")) {}
				//	if (ImGui::MenuItem("Copy", "CTRL+C")) {}
				//	if (ImGui::MenuItem("Paste", "CTRL+V")) {}
				//	ImGui::EndMenu();
				//}
				ImGui::EndMainMenuBar();
			}
		}

		// scene
		{
			ImGui::SetNextWindowPos(ImVec2(0,25));

			ImGuiWindowFlags window_flags =  ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollWithMouse;
			ImGui::Begin("EditMode",0,window_flags);
			ImGui::BeginChild(" ");
			edit_mode->Render();
			auto tex = edit_mode->GetTexture();
			ImGui::Image(reinterpret_cast<void*>(tex), ImVec2(edit_mode->width, edit_mode->height), ImVec2(0, 1), ImVec2(1, 0));
			ImGui::EndChild();
			ImGui::End();
		}


		ImGui::ShowDemoWindow();

		// assets 
		if(1)
		{
			ImGui::SetNextWindowPos(ImVec2(0, 660));
			ImGui::SetNextWindowSize(ImVec2(1750, 350));
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollWithMouse;
			ImGui::Begin("Assets", 0, window_flags);


			int selected = 0;
			{
				
				//ImGui::SetNextWindowSize(ImVec2(250, 100));
				ImGui::BeginChild("left pane", ImVec2(150, 0), true);
				for (int i = 0; i < 100; i++)
				{
					char label[128];
					sprintf(label, "MyObject %d", i);
					if (ImGui::Selectable(label, selected == i))
						selected = i;
				}
				ImGui::EndChild();
			}


			ImGui::End();
		}
		if (0)
		// inspector 
		{
			ImGui::SetNextWindowPos(ImVec2(830, 10));
			ImGui::SetNextWindowSize(ImVec2(400, 635));
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollWithMouse;
			ImGui::Begin("Inspector", 0, window_flags);
			ImGui::End();
		}
		
		// scene properties
		if (0)
		{
			ImGui::Begin("menu");
			ImGui::Text("main camera");
			//ImGui::DragFloat3();
			ImGui::End();
		}
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(base_window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(base_window);
	}



}


void Editor::processInput(GLFWwindow* window)
{

	float delta_time=ImGui::GetIO().Framerate/3000.0f;
	// drag with right mouse button 
	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2))
		edit_mode->render_engine->GetCam()->ProcessMouseMovement(xoffset, yoffset);
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		edit_mode->render_engine->GetCam()->ProcessKeyboard(Direction::FORWARD, delta_time);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		edit_mode->render_engine->GetCam()->ProcessKeyboard(Direction::BACKWARD, delta_time);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		edit_mode->render_engine->GetCam()->ProcessKeyboard(Direction::LEFT, delta_time);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		edit_mode->render_engine->GetCam()->ProcessKeyboard(Direction::RIGHT, delta_time);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		edit_mode->render_engine->GetCam()->ProcessKeyboard(Direction::UP, delta_time);
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		edit_mode->render_engine->GetCam()->ProcessKeyboard(Direction::DOWN, delta_time);

	//std::cout << deltaTime<<render_engine->GetCam()->Position.x << " " << render_engine->GetCam()->Position.y << " " << render_engine->GetCam()->Position.z << "\n";
}

uint32_t EditMode::GetTexture()
{
	return render_engine->GetTexture();
}

void EditMode::Render()
{
	render_engine->Render();
}
