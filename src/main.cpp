#include "renderer_opengl.h"
#include <iostream>


int main()
{

    OpenGLRenderer renderer{};
    renderer.init();

    //Render loop
    while (!renderer.shouldClose()) {
        //processInput()
        //physics

        //render
        renderer.render();
    }

    glfwTerminate();
    return 0;
}

