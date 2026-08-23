#include "Window.hpp"

#include "Logging.hpp"

#include "glad/glad.h"
#include "glfw/glfw3.h"

namespace Neon
{
    Window::Window()
        : Window(800, 600) {}

    Window::Window(unsigned int xSize, unsigned int ySize)
    {
        m_sizeX = xSize;
        m_sizeY = ySize;

        if (!glfwInit())
        {
            Logging::Error("GLFW init failed");
            return;
        }
        else
        {
            Logging::Info("GLFW initialized");
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
        glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);

        m_window = std::unique_ptr<GLFWwindow, GLFWWindowDeleter>(glfwCreateWindow(m_sizeX, m_sizeY, "Neon3D", NULL, NULL));
        if (m_window == NULL)
        {
            Logging::Error("Failed to create GLFW window");
            glfwTerminate();
            return;
        }
        else
        {
            Logging::Info("GLFW window created");
        }
        glfwSetWindowUserPointer(m_window.get(), this);
        glfwMakeContextCurrent(m_window.get());

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            Logging::Error("Failed to initialize GLAD");
            return;
        }
        else
        {
            Logging::Info("GLAD initializedn");
        }

        glViewport(0, 0, m_sizeX, m_sizeY);
        glfwSetFramebufferSizeCallback(m_window.get(), framebuffer_size_callback);
        glEnable(GL_DEPTH_TEST);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
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

        glClearColor(44 / 250.0f, 44 / 250.0f, 44 / 250.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    bool Window::shouldWindowsClose()
    {
        if (!m_window)
            return true;

        return glfwWindowShouldClose(m_window.get());
    }

    int Window::getHeight()
    {
        return m_sizeY;
    }

    float Window::getAspectRatio()
    {
        return static_cast<float>(m_sizeX) / static_cast<float>(m_sizeY);
    }

    int Window::getWidth()
    {
        return m_sizeX;
    }

} // namespace Neon
