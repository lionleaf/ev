#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "physics_2d.h"

class OpenGLRenderer {
public:
    void init();
    void clear();
    void finishRendering();
    void render();
    bool shouldClose();
    void drawCircle(phys::Circle circle);
private:
    GLFWwindow *mWindow;
    GLuint shaderProgram{};
    GLuint VAO{};
};



