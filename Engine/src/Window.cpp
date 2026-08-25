#include "Window.hpp"

#include "Assert.hpp"
#include "Input.hpp"
#include "Logging.hpp"
#include "Events/WindowClosedEvent.hpp"
#include "Events/WindowFocusedEvent.hpp"
#include "Events/WindowLostFocusEvent.hpp"
#include "Events/WindowResizedEvent.hpp"

#include "glad/glad.h"
#include "glfw/glfw3.h"

namespace Neon
{
    Window::Window(Input &input, EventBus &eventBus)
        : Window(800, 600, input, eventBus) {}

    Window::Window(
        unsigned int xSize, unsigned int ySize,
        Input &input, EventBus &eventBus) : m_input(input), m_eventBus(eventBus)
    {
        m_width = xSize;
        m_framebufferWidth = xSize;

        m_height = ySize;
        m_framebufferHeight = ySize;

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

        m_window = std::unique_ptr<GLFWwindow, GLFWWindowDeleter>(glfwCreateWindow(m_width, m_height, "Neon3D", NULL, NULL));
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

        glViewport(0, 0, m_width, m_height);
        glfwSetFramebufferSizeCallback(m_window.get(), framebufferSizeCallback);
        glEnable(GL_DEPTH_TEST);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        glfwSetKeyCallback(m_window.get(), keysCallback);
        glfwSetCursorPosCallback(m_window.get(), mouseMovmentCallback);
        glfwSetMouseButtonCallback(m_window.get(), mouseButtonCallback);
        glfwSetScrollCallback(m_window.get(), mouseScrollCallback);

        glfwSetWindowSizeCallback(m_window.get(), windowResizedCallback);
        glfwSetWindowCloseCallback(m_window.get(), windowClosedCallback);
        glfwSetWindowFocusCallback(m_window.get(), windowFocusedCallback);
    }

    Window::~Window()
    {
        Logging::Info("Window deleted");
        m_window.reset();
        glfwTerminate();
    }

    void Window::GLFWWindowDeleter::operator()(GLFWwindow *window)
    {
        if (window)
            glfwDestroyWindow(window);
    }

    void Window::framebufferSizeCallback(GLFWwindow *window, int width, int height)
    {
        Window *self = static_cast<Window *>(glfwGetWindowUserPointer(window));

        self->m_framebufferWidth = width;
        self->m_framebufferHeight = height;

        glViewport(0, 0, width, height);
    }

    void Window::render()
    {
        glfwSwapBuffers(m_window.get());

        glClearColor(44 / 250.0f, 44 / 250.0f, 44 / 250.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Window::update()
    {
        if (m_input.getCursorMode() != m_cursorMode)
        {
            m_cursorMode = m_input.getCursorMode();

            glfwSetInputMode(
                m_window.get(),
                GLFW_CURSOR,
                toGlfwCursorMode(m_cursorMode));
        }
    }

    bool Window::shouldWindowsClose()
    {
        if (!m_window)
            return true;

        return glfwWindowShouldClose(m_window.get());
    }

    int Window::getHeight()
    {
        return m_height;
    }

    float Window::getAspectRatio()
    {
        return static_cast<float>(m_framebufferWidth) / static_cast<float>(m_framebufferHeight);
    }

    int Window::getWidth()
    {
        return m_width;
    }

    void Window::keysCallback(GLFWwindow *glfwWindow, int key, int scancode, int action, int mods)
    {

        Window *window = static_cast<Window *>(glfwGetWindowUserPointer(glfwWindow));
        window->m_input.onKey(key, scancode, action, mods);
    }

    void Window::mouseMovmentCallback(GLFWwindow *glfwWindow, double x, double y)
    {

        Window *window = static_cast<Window *>(glfwGetWindowUserPointer(glfwWindow));
        window->m_input.onMouseMovement(x, y);
    }

    void Window::mouseButtonCallback(GLFWwindow *glfwWindow, int button, int action, int mods)
    {

        Window *window = static_cast<Window *>(glfwGetWindowUserPointer(glfwWindow));
        window->m_input.onMouseButton(button, action, mods);
    }
    void Window::mouseScrollCallback(GLFWwindow *glfwWindow, double xOffset, double yOffset)
    {

        Window *window = static_cast<Window *>(glfwGetWindowUserPointer(glfwWindow));
        window->m_input.onMouseScroll(xOffset, yOffset);
    }

    void Window::windowResizedCallback(GLFWwindow *glfwWindow, int width, int height)
    {
        Window *window = static_cast<Window *>(glfwGetWindowUserPointer(glfwWindow));

        window->m_width = width;
        window->m_height = height;

        window->m_eventBus.emit(WindowResizedEvent({width, height}));
    }
    void Window::windowClosedCallback(GLFWwindow *glfwWindow)
    {

        Window *window = static_cast<Window *>(glfwGetWindowUserPointer(glfwWindow));
        window->m_eventBus.emit(WindowClosedEvent());
    }

    void Window::windowFocusedCallback(GLFWwindow *glfwWindow, int focused)
    {

        Window *window = static_cast<Window *>(glfwGetWindowUserPointer(glfwWindow));
        if (focused)
            window->m_eventBus.emit(WindowFocusedEvent());
        else
            window->m_eventBus.emit(WindowLostFocusEvent());
    }

    int Window::toGlfwCursorMode(Input::CursorMode mode)
    {
        int glfwMode;
        switch (mode)
        {
        case Input::CursorMode::Normal:
            glfwMode = GLFW_CURSOR_NORMAL;
            break;
        case Input::CursorMode::Hidden:
            glfwMode = GLFW_CURSOR_HIDDEN;
            break;
        case Input::CursorMode::Disabled:
            glfwMode = GLFW_CURSOR_DISABLED;
            break;
        default:
            NEON_ASSERT(false, "Invalid Cursor mode passed");
            break;
        }

        return glfwMode;
    }

} // namespace Neon
