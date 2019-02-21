#include "renderer_opengl.h"
#include <iostream>

using namespace phys;
void CheckOpenGLError(const char* stmt, const char* fname, int line)
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        printf("OpenGL error %08x, at %s:%i - for %s\n", err, fname, line, stmt);
        abort();
    }
}

#define _DEBUG
#ifdef _DEBUG
#define GL(stmt) do { \
            stmt; \
            CheckOpenGLError(#stmt, __FILE__, __LINE__); \
        } while (0)
#else
#define GL(stmt) stmt
#endif

static const char* SHADER_VERTEX_SOURCE = R"raw(
    #version 410 core

    layout (location = 0) in vec3 pos;

    void main()
    {
        gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
    }
)raw";

static const char* SHADER_FRAGMENT_SOURCE = R"raw(
    #version 410 core
    out vec4 FragColor;

    void main()
    {
        FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    }
)raw";


void OpenGLRenderer::init() {
    //GLFW init 
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //Needed for Mac

    //Window creation
    mWindow = glfwCreateWindow(800, 600, "EvoView", nullptr, nullptr);
    if (mWindow == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(mWindow);

    
    //Glad init
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cout << "Failed to init GLAD" << std::endl;
    }


    //Viewport setup
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(mWindow, 
        [](GLFWwindow*, int width, int height)
        {
            GL( glViewport(0, 0, width, height) );
        }
    );

    //compile shaders
    GLuint vertexShader{};
    vertexShader =  glCreateShader(GL_VERTEX_SHADER) ;
    GL( glShaderSource(vertexShader, 1, &SHADER_VERTEX_SOURCE, nullptr) );
    GL( glCompileShader(vertexShader) );

    GLuint fragmentShader{};
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    GL( glShaderSource(fragmentShader, 1, &SHADER_FRAGMENT_SOURCE, nullptr));
    GL( glCompileShader(fragmentShader));

    shaderProgram = glCreateProgram();
    GL( glAttachShader(shaderProgram, vertexShader));
    GL( glAttachShader(shaderProgram, fragmentShader));
    GL( glLinkProgram(shaderProgram));
    
    GL( glUseProgram(shaderProgram));


    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };

    GL( glGenVertexArrays(1, &VAO));
    GL( glBindVertexArray(VAO));

    unsigned int VBO;
    GL( glGenBuffers(1, &VBO));
    GL( glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GL( glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
    GL( glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0));
    GL( glEnableVertexAttribArray(0));
} 

void OpenGLRenderer::clear() {
    GL(glClearColor(0.4f, 0.2f, 0.4f, 1.0f));
    GL(glClear(GL_COLOR_BUFFER_BIT));
}
void OpenGLRenderer::finishRendering() {

    glfwPollEvents();
    glfwSwapBuffers(mWindow);
}

bool OpenGLRenderer::shouldClose() {

    return glfwWindowShouldClose(mWindow);
}

void OpenGLRenderer::drawCircle(Circle circle)
{
    float scale = 1 / 10.0f;
    float vertices[] = {
        (circle.pos.x - circle.radius) * scale, (circle.pos.y - circle.radius) * scale, 0.0f,
        (circle.pos.x + circle.radius) * scale, (circle.pos.y - circle.radius) * scale , 0.0f,
        (circle.pos.x                ) * scale, (circle.pos.y + circle.radius) * scale , 0.0f,
    };

    GLuint vao;
    GL( glGenVertexArrays(1, &vao));
    GL( glBindVertexArray(vao));

    GLuint vbo;
    GL( glGenBuffers(1, &vbo));
    GL( glBindBuffer(GL_ARRAY_BUFFER, vbo));
    GL( glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
    GL( glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0));
    GL( glEnableVertexAttribArray(0));

    GL(glUseProgram(shaderProgram));
    GL(glBindVertexArray(vao));
    GL(glDrawArrays(GL_TRIANGLES, 0, 3));
}




