#ifndef NEON_WINDOW
#define NEON_WINDOW

#include <memory>

struct GLFWwindow;

namespace Neon
{
    class Window
    {
    public:
        Window(unsigned int xSize, unsigned int ySize);
        Window();
        ~Window();

        void render();
        bool shoudWindowsClose();

        int getWidth();
        int getHeight();

        float getAspectRatio();

        GLFWwindow *getGlfwWindow()
        {
            return m_window.get();
        }

    private:
        struct GLFWWindowDeleter
        {
            void operator()(GLFWwindow *window);
        };
        static void framebuffer_size_callback(GLFWwindow *window, int width, int height);

        std::unique_ptr<GLFWwindow, GLFWWindowDeleter> m_window;

        int m_sizeX = 800;
        int m_sizeY = 600;
    };
}

#endif