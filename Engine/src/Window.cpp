#include <iostream>

#include "Window.hpp"
#include "glad/glad.h"
#include "glfw/glfw3.h"

namespace Neon
{
    Window::Window(int xSize, int ySize)
    {
        m_sizeX = xSize;
        m_sizeY = ySize;
        Window();
    }

    Window::Window()
    {
        if (!glfwInit())
        {
            std::cout << "\033[31m[NEON][ERROR] - GLFW init failed\033[0m" << std::endl;
            return;
        }
        else
        {
            std::cout << "\033[32m[NEON][INFO] - GLFW initialized\033[0m\n";
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
        glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);

        m_window = std::unique_ptr<GLFWwindow, GLFWWindowDeleter>(glfwCreateWindow(m_sizeX, m_sizeY, "Neon3D", NULL, NULL));
        if (m_window == NULL)
        {
            std::cout << "\033[31m[NEON][ERROR] - Failed to create GLFW window\033[0m" << std::endl;
            glfwTerminate();
            return;
        }
        else
        {
            std::cout << "\033[32m[NEON][INFO] - GLFW window created\033[0m\n";
        }
        glfwSetWindowUserPointer(m_window.get(), this);
        glfwMakeContextCurrent(m_window.get());

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "\033[31m[NEON][ERROR] - Failed to initialize GLAD\033[0m" << std::endl;
            return;
        }
        else
        {
            std::cout << "\033[32m[NEON][INFO] - GLAD iinitializedn\033[0m" << std::endl;
        }

        glViewport(0, 0, m_sizeX, m_sizeY);
        glfwSetFramebufferSizeCallback(m_window.get(), framebuffer_size_callback);
    }

    Window::~Window()
    {
        m_window.reset();
        glfwTerminate();
    }

    void Window::GLFWWindowDeleter::operator()(GLFWwindow *window)
    {
        if (window)
            glfwDestroyWindow(window);
    }

    void Window::framebuffer_size_callback(GLFWwindow *window, int width, int height)
    {
        Window *self = static_cast<Window *>(glfwGetWindowUserPointer(window));

        self->m_sizeX = width;
        self->m_sizeY = height;

        glViewport(0, 0, width, height);
    }

    void Window::render()
    {
        glfwSwapBuffers(m_window.get());
        glfwPollEvents();

        glClearColor(44 / 250.0f, 44 / 250.0f, 44 / 250.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    bool Window::shoudWindowsClose()
    {
        if (!m_window)
            return true;

        return glfwWindowShouldClose(m_window.get());
    }

} // namespace Neon
