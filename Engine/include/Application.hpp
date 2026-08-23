#ifndef NEON_APPLICATION
#define NEON_APPLICATION

#include "Window.hpp"
#include "Time.hpp"
#include "Input.hpp"

namespace Neon
{
    class Application
    {
    public:
        Application();
        virtual ~Application();

        void run();

    protected:
        virtual void onStart() {};
        virtual void onUpdate(float dt) {};
        virtual void onRender() {};
        virtual void onEnd() {};

        Window &getWindow() { return m_window; }
        const Window &getWindow() const { return m_window; }

        Input &getInput() { return m_input; }
        const Input &getInput() const { return m_input; }

        Time &getTime() { return m_time; }
        const Time &getTime() const { return m_time; }

    private:
        Window m_window;
        Input m_input;
        Time m_time;
    };
}

#endif