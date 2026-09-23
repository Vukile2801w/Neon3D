#ifndef NEON_APPLICATION
#define NEON_APPLICATION

#include "Assets/AssetManager.hpp"

#include "Rendering/Renderer.hpp"
#include "Rendering/Window.hpp"

#include "Events/EventBus.hpp"

#include "Debug/ImGuiLayer.hpp"

#include "Camera.hpp"
#include "Input.hpp"
#include "Time.hpp"

namespace Neon
{
    class Application
    {
    public:
        Application();
        virtual ~Application();

        void run();

        Window &getWindow() { return m_window; }
        const Window &getWindow() const { return m_window; }

        Input &getInput() { return m_input; }
        const Input &getInput() const { return m_input; }

        Time &getTime() { return m_time; }
        const Time &getTime() const { return m_time; }

        EventBus &getEventBus() { return m_eventBus; }
        const EventBus &getEventBus() const { return m_eventBus; }

        Renderer &getRenderer() { return m_renderer; }
        const Renderer &getRenderer() const { return m_renderer; }

        AssetManager &getAssetManager() { return m_assetManager; }
        const AssetManager &getAssetManager() const { return m_assetManager; }

        ImGuiLayer &getImGui() { return m_imGui; }
        const ImGuiLayer &getImGui() const { return m_imGui; }

        bool isGuiEnabled() const { return m_isGuiEnabled; }
        void setGuiEnabled(bool isOn) { m_isGuiEnabled = isOn; }
        void toggleGuiEnabled() { m_isGuiEnabled = !m_isGuiEnabled; }

    protected:
        virtual void onStart() {};
        virtual void onUpdate(float dt) {};
        virtual void onRender() {};
        virtual void onEnd() {};

    private:
        bool m_isGuiEnabled{false};

        Renderer m_renderer;
        EventBus m_eventBus;
        Window m_window;
        Camera m_camera;
        Input m_input;
        Time m_time;
        AssetManager m_assetManager;
        ImGuiLayer m_imGui;
    };
}

#endif