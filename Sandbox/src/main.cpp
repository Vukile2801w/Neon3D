#include "Window.hpp"

int main()
{
    std::unique_ptr<Neon::Window> window =
        std::make_unique<Neon::Window>();

    while (!window->shoudWindowsClose())
    {
        window->render();
    }

    return 0;
}