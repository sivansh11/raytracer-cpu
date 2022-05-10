#ifndef WINDOW_H
#define WINDOW_H

#include "debug.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window
{
public:
    Window(int width, int height, const char *name)
    {
        ASSERT(glfwInit() == GLFW_TRUE, "GLFW failed to initialize!");

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(width, height, name, NULL, NULL);
        ASSERT(window != nullptr, "GLFW failed to create window");

        glfwMakeContextCurrent(window);

        gladLoadGL();
    }
    ~Window()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
    void startFrame()
    {
        glfwPollEvents();
    }
    void endFrame()
    {
        glfwSwapBuffers(window);
    }
    GLFWwindow* getGLFWwindow() { return window; }
    bool shouldClose() { return glfwWindowShouldClose(window); }

private:
    GLFWwindow* window;
};

#endif