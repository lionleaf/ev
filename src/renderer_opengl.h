#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class OpenGLRenderer {
public:
    void init();
    void render();
    bool shouldClose();
private:
    GLFWwindow *mWindow;
    GLuint shaderProgram{};
    GLuint VAO{};
};



