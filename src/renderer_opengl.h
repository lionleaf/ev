#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "physics_2d.h"

class OpenGLRenderer {
public:
    void init();
    void clear();
    void drawCircle(phys::Circle circle);
    void finishRendering();
    bool shouldClose();
private:
    GLFWwindow *mWindow;
    GLuint m_shader_program_flat{};
};



