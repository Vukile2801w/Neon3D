#include "Application.hpp"

#include "Debug/Logging.hpp"

namespace Neon
{
    Application::Application() : m_input(m_eventBus), m_window(m_input, m_eventBus),
                                 m_time(), m_camera(), m_renderer(m_camera, m_window), m_assetManager(),
                                 m_imGui(this)
    {
    }

    Application::~Application()
    {
    }

    void Application::run()
    {
        Logging::Init("Neon");
        Logging::Info("Application starting");
        onStart();
        Logging::Info("onStart finished");

        while (!m_window.shouldWindowsClose())
        {

            m_time.beginFrame();
            m_input.handleInput();
            m_window.update();
            m_imGui.begin();

            onUpdate(m_time.getDeltaTime());

            onRender();
            m_imGui.end();
            m_window.render();
        }

        Logging::Info("Application loop ended");
        onEnd();
    }

}