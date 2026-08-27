#ifndef NEON_WINDOW
#define NEON_WINDOW

#include <memory>

#include "Events/EventBus.hpp"
#include "Input.hpp"

struct GLFWwindow;

namespace Neon
{
    class Window
    {
    public:
        Window(unsigned int xSize, unsigned int ySize, Input &input, EventBus &eventBus);
        Window(Input &input, EventBus &eventBus);
        ~Window();

        void update();

        void render();
        bool shouldWindowsClose();

        int getWidth();
        int getHeight();

        float getAspectRatio();

    private:
        struct GLFWWindowDeleter
        {
            void operator()(GLFWwindow *window);
        };
        static void framebufferSizeCallback(GLFWwindow *window, int width, int height);

        std::unique_ptr<GLFWwindow, GLFWWindowDeleter> m_window;

        int toGlfwCursorMode(Input::CursorMode mode);

        // ==== EVENTS === //

        static void keysCallback(GLFWwindow *glfwWindow, int key, int scancode, int action, int mods);
        static void mouseMovmentCallback(GLFWwindow *glfwWindow, double x, double y);
        static void mouseButtonCallback(GLFWwindow *glfwWindow, int button, int action, int mods);
        static void mouseScrollCallback(GLFWwindow *glfwWindow, double xOffset, double yOffset);

        // --------------- //

        static void windowResizedCallback(GLFWwindow *window, int width, int height);
        static void windowClosedCallback(GLFWwindow *window);
        static void windowFocusedCallback(GLFWwindow *window, int focused);

        int m_width = 800;
        int m_height = 600;

        int m_framebufferWidth = 800;
        int m_framebufferHeight = 600;

        Input::CursorMode m_cursorMode;

        Input &m_input;
        EventBus &m_eventBus;
    };
}

#endif