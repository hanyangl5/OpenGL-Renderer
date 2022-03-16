#include <glad/glad.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <array>
#include <imgui.h>
#include <nfd.h>

#include "Editor.h"
#include "Log.h"
#include "utils.h"

Editor::Editor(const char *_name, uint32_t _width, uint32_t _height)
    : width(_width), height(_height) {
  width = _width;
  height = _height;
  if (glfwInit() != GLFW_TRUE) {
    glfwTerminate();
    Log::Log("failed to init glfw");
  };

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4.6);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4.6);
  base_window = glfwCreateWindow(width, height, "Dredgen-gl", nullptr, nullptr);

  if (!base_window) {
    glfwTerminate();
    Log::Err("failed to init window");
  }

  glfwMakeContextCurrent(base_window);
  glfwSwapInterval(0); // Enable vsync

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    Log::Err("Failed to initialize GLAD");
  }
  edit_mode = std::make_shared<EditMode>(1280, 900);
  //edit_mode = std::make_shared<EditMode>(1800, 900);
}
Editor::~Editor() {
  glfwDestroyWindow(base_window);
  glfwTerminate();
  Log::Log("destroyed");
}

void Editor::Run() {
  scene_cam = std::make_shared<Camera>(glm::vec3(2.0f, -1.0f, 5.0f));

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;

  ImGui::StyleColorsLight();
  io.Fonts->AddFontFromFileTTF("../resources/fonts/Consolas.ttf", 17);
  const char *glsl_version = "#version 130";
  ImGui_ImplGlfw_InitForOpenGL(base_window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);
  ImVec4 clear_color = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);
  bool show_demo_window = true;
  bool show_another_window = false;
  // Main loop
  while (!glfwWindowShouldClose(base_window)) {
    glfwPollEvents();

    // process user input
    {
      float xpos = io.MousePos.x;
      float ypos = io.MousePos.y;
      {
        if (firstMouse) {
          lastX = xpos;
          lastY = ypos;
          firstMouse = false;
        }

        xoffset = xpos - lastX;
        yoffset =
            lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;
      }
      processInput(base_window);
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    // menubar

    if (1) {
      if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Import Asset")) {
          if (ImGui::MenuItem("Import from file")) {
            nfdchar_t *outPath = NULL;
            nfdresult_t result = NFD_OpenDialog(NULL, NULL, &outPath);

            // check name/type/dir
            if (result == NFD_OKAY) {

              std::string objectname =
                  "Object " + std::to_string(objects.size());
              std::string dir(outPath);
              free(outPath);
              std::string suffix_str = dir.substr(dir.find_last_of('.') + 1);

              ObjectType type;
              if (suffix_str == "gltf") {
                type = ObjectType::model_gltf;
              } else if (suffix_str == "obj") {
                type = ObjectType::model_obj;
              } else if (suffix_str == "fbx") {
                type = ObjectType::model_fbx;
              } else if (suffix_str == "jpg" || suffix_str == "jpeg") {
                type = ObjectType::texture_jpg;
              } else if (suffix_str == "png") {
                type = ObjectType::texture_png;
              } else if (suffix_str == "bmp") {
                type = ObjectType::texture_bmp;
              } else if (suffix_str == "tga") {
                type = ObjectType::texture_tga;
              } else if (suffix_str == "psd") {
                type = ObjectType::texture_psd;
              } else {
                type = ObjectType::none_type;
              }
              if (type != ObjectType::none_type) {
                objects.push_back(DObject{objectname, dir, type});
                added.push_back(false);
              }

            } else if (result == NFD_CANCEL) {
            } else {
              Log::Log("Error:", NFD_GetError());
            }
            // ShellExecute(NULL, "open", "C:", NULL, NULL, SW_SHOWDEFAULT);
          }

          ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
      }
    }

    // editmode
    {
      ImGui::SetNextWindowPos(ImVec2(0, 25));

      ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar |
                                      ImGuiWindowFlags_AlwaysAutoResize;
      ImGui::Begin("EditMode", 0, window_flags);
      edit_mode->Render();
      void *nup = nullptr;
      auto tex = reinterpret_cast<void *>(edit_mode->GetTexture());
      ImGui::Image(tex, ImVec2(edit_mode->width, edit_mode->height),
                   ImVec2(0, 1), ImVec2(1, 0));
      ImGui::End();
    }
    // assets
    if (1) {
      //ImGui::SetNextWindowPos(ImVec2(0, 665));
      //ImGui::SetNextWindowSize(ImVec2(815, 330));
      //ImGuiWindowFlags window_flags =
      //    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollWithMouse;
	  ImGuiWindowFlags window_flags =ImGuiWindowFlags_NoScrollWithMouse;
      ImGui::Begin("Assets", 0, window_flags);

      static int selected = -1;
      {

        ImGui::BeginChild("left pane", ImVec2(300, 0), true);
        for (int i = 0; i < objects.size(); i++) {
          char label[128];
          // sprintf("%s",objects[i].directory.c_str());
          sprintf(label, "%s", objects[i].name.c_str());
          if (ImGui::Selectable(label, selected == i))
            selected = i;
        }
        ImGui::EndChild();
      }

      ImGui::SameLine();
      if (selected != -1) {
        ImGui::BeginGroup();
        ImGui::BeginChild(
            "item view",
            ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1
                                                             // line below us
        ImGui::Text("Object %d", selected);
        ImGui::Separator();
        if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None)) {
          if (ImGui::BeginTabItem("Description")) {
            ImGui::TextWrapped("");
            // std::cout << selected;

            ImGui::Text("name: %s", objects[selected].name.c_str());
            ImGui::Text("type: %s", objects[selected].GetTypeStr().c_str());
            ImGui::Text("directory: %s", objects[selected].directory.c_str());
            ImGui::EndTabItem();
          }
          ImGui::EndTabBar();
        }
        ImGui::EndChild();
        if (selected != -1)
          if ((added[selected] == false) && ImGui::Button("add to scene")) {
            edit_mode->AddModel(objects[selected].name,
                                objects[selected].directory);
            added[selected] = true;
          }
        ImGui::EndGroup();
      }
      ImGui::End();
    }
    // scene properties
    if (1) {
      ImGui::Begin("Infos");
      // ImGui::Text("main camera");
      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                  1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
      ImGui::Text("cursor pos: %.3f %.3f", io.MousePos.x, io.MousePos.y);

      std::array<float, 3> cam_pos{scene_cam->Position[0],
                                   scene_cam->Position[1],
                                   scene_cam->Position[2]};
      ImGui::SliderFloat3("cam pos", cam_pos.data(), -10.0f, 10.0f);
      ImGui::InputFloat("edit cam speed",
                        &edit_mode->render_engine->GetCam()->MovementSpeed);
      scene_cam->Position = glm::vec3{cam_pos[0], cam_pos[1], cam_pos[2]};
      ImGui::End();
    }
    if (1) {
      edit_mode->GetSceneStats();
    }

    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(base_window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                 clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(base_window);
  }
}

void Editor::processInput(GLFWwindow *window) {

  float delta_time = ImGui::GetIO().Framerate / 3000.0f;
  //Log::Log(delta_time);
  // drag with right mouse button
  //1seond 1m
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2))
    edit_mode->render_engine->GetCam()->ProcessMouseMovement(xoffset, yoffset);
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    edit_mode->render_engine->GetCam()->ProcessKeyboard(Direction::FORWARD,
                                                        delta_time);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    edit_mode->render_engine->GetCam()->ProcessKeyboard(Direction::BACKWARD,
                                                        delta_time);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    edit_mode->render_engine->GetCam()->ProcessKeyboard(Direction::LEFT,
                                                        delta_time);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    edit_mode->render_engine->GetCam()->ProcessKeyboard(Direction::RIGHT,
                                                        delta_time);
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    edit_mode->render_engine->GetCam()->ProcessKeyboard(Direction::UP,
                                                        delta_time);
  if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    edit_mode->render_engine->GetCam()->ProcessKeyboard(Direction::DOWN,
                                                        delta_time);
}

uint32_t EditMode::GetTexture() { return render_engine->GetTexture(); }

void EditMode::Render() {
  render_engine->Update();
  render_engine->Render();
}

void EditMode::AddModel(std::string name, std::string path) {
  render_engine->AddModel(name, path);
}

void EditMode::GetSceneStats() { render_engine->GetSceneStats(); }

// uint32_t EditMode::RenderAt(std::shared_ptr<Camera> cam)
//{
//	return render_engine->RenderAt(cam);
//}
