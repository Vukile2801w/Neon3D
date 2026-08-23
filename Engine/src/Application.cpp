#include "Application.hpp"

#include "Logging.hpp"

namespace Neon
{
    Application::Application() : m_window(), m_input(&m_window), m_time()
    {
    }

    Application::~Application()
    {
    }

    void Application::run()
    {
        Logging::Info("Application starting");
        onStart();
        Logging::Info("onStart finished");

        while (!m_window.shouldWindowsClose())
        {

            m_time.beginFrame();
            m_input.handleInput();

            onUpdate(m_time.getDeltaTime());

            onRender();
            m_window.render();
        }

        Logging::Info("Application loop ended");
        onEnd();
    }
}