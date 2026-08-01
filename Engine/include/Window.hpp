#ifndef NEON_WINDOW
#define NEON_WINDOW

#include <memory>

struct GLFWwindow;

namespace Neon
{
    class Window
    {
    public:
        Window(int xSize, int ySize);
        Window();
        ~Window();

        void render();
        bool shoudWindowsClose();

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