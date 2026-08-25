#include "Application.hpp"

#include "Logging.hpp"

namespace Neon
{
    Application::Application() : m_input(m_eventBus), m_window(m_input, m_eventBus), m_time(), m_camera(), m_renderer(m_camera, m_window)
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
            m_window.update();

            onUpdate(m_time.getDeltaTime());

            onRender();
            m_window.render();
        }

        Logging::Info("Application loop ended");
        onEnd();
    }
}