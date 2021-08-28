#include "GLFW/glfw3.h"
#include <iostream>

static void error_callback(int code, const char *description)
{
    std::cerr << "glfw error code: " << code << " (" << description << ")" << std::endl;
}

static float randomNumber()
{
    return float(rand()) / float(RAND_MAX);
}

static void render(GLFWwindow *window)
{
    glfwMakeContextCurrent(window);

    glClearColor(randomNumber(), randomNumber(), randomNumber(), 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

int main()
{
    if (!glfwInit())
    {
        return -1;
    }

    glfwSetErrorCallback(error_callback);

    int major, minor, revision;
    glfwGetVersion(&major, &minor, &revision);

    std::cout << "Running against GLFW " << major << "." << minor << "." << revision << std::endl;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    GLFWwindow *window = glfwCreateWindow(640, 480, "OpenGL ditty", NULL, NULL);

    while (!glfwWindowShouldClose(window))
    {
        render(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}
