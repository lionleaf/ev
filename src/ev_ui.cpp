//
//  ui.cpp
//  ev
//
//  Created by Andreas Løve Selvik on 4/2/19.
//

#include "ev_ui.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
namespace ev {
namespace ev_ui {

void generation_info(int nr, int creature_nr, std::vector<float> fitness) {
  ImGui::Begin("Generation");
  ImGui::Text("Generation nr: %i, creature: %i", nr, creature_nr);
  ImGui::PlotLines("fitness", fitness.data(), fitness.size());
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
              1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  ImGui::End();
}

void camera_control(Vec2f& pos) {
  ImGui::Begin("Camera");
  ImGui::SliderFloat("x", &pos.x, -100.0f, 100.0f);
  ImGui::End();
}

void main_ui() {
  // Some copy-pasted example UI code.

  static float f = 0.0f;  // Proof of concept, we don't want to keep state here
                          // in static vars
  static int counter = 0;
  static bool show_another_window = true;

  if (show_another_window) {
    ImGui::Begin("Hello, world!");
    ImGui::Text("Hallais Daniel.");
    ImGui::End();
  }

  ImGui::Begin("Windowowowow");

  ImGui::Checkbox(
      "Daniel Window",
      &show_another_window);  // Edit bools storing our window open/close state

  ImGui::SliderFloat("float", &f, 0.0f,
                     1.0f);  // Edit 1 float using a slider from 0.0f to 1.0f

  if (ImGui::Button("Button"))  // Buttons return true when clicked (most
                                // widgets return true when edited/activated)
    counter++;
  ImGui::SameLine();
  ImGui::Text("counter = %d", counter);

  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
              1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  ImGui::End();
}

void start_frame() {
  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void draw() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void init(GLFWwindow* window) {
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard
  // Controls io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable
  // Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsClassic();

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  const char* glsl_version_string = "#version 410";
  ImGui_ImplOpenGL3_Init(glsl_version_string);
}

void destroy() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

}  // namespace ev_ui
}  // namespace ev
