#pragma once
#include "ev_math.h"
#include <vector>
#include <glad/glad.h>
// GLFW must be included after glad.h
#include <GLFW/glfw3.h>

namespace ev_ui {

// Called once to init
void init(GLFWwindow* window);

// Called once before each frame, sets up imgui
void start_frame();

// Called once per frame to draw the main parts of the UI
// Because of the immediate mode structure of ImGui
// other parts of the UI can be drawn in different functions
// called from different locations
void main_ui();
  
void camera_control(Vec2f& pos);

void generation_info(int nr, int creature_nr, std::vector<float> fitness);

// Called at the end of frame to draw and display the UI
void draw();

// Tear-down  TODO: Convert to RAII
void destroy();

}  // namespace ev_ui
