#include <iterator>
#include <chrono>
#include <sstream>
#include <random>
#include <thread>
#include "Neon.hpp"
#include "gtc/matrix_transform.hpp"

#include "Cube.hpp"

// Enums
using Neon::AssetManager;
using Neon::EventBus;
using Neon::Input;
using Neon::Logging;
using Neon::LoggingLevel;
using Neon::ShaderDataType;
using Neon::TextureFilter;

class Sandbox : public Neon::Application
{
public:
    Sandbox() : m_camera(getRenderer().getCamera()), m_scene(this)
    {
    }
    ~Sandbox()
    {
    }

protected:
    Cube *m_monkey = nullptr;

    void findMonkey()
    {
        for (const auto &object : m_scene.getGameObjects())
        {
            if (object->name == "Monkey")
            {
                m_monkey = dynamic_cast<Cube *>(object.get());
                return;
            }
        }
    }

    void onStart() override
    {
        EventBus &eventBus = getEventBus();
        AssetManager &assetManager = getAssetManager();

        Neon::GameObjectFactory::registerType(
            "Cube",
            &Cube::create);

        m_camera.position.y = 1.3f;
        m_camera.FOV = 60;

        // spawnCubes(121);

        Neon::SceneSerializer::load(m_scene, "Sandbox/assets/scene2.neon", assetManager);
        getImGui().attachScene(&m_scene);

        eventBus.subscribe<Neon::MouseMovedEvent>([this](const Neon::MouseMovedEvent &event)
                                                  { inputCallback(event); });

        eventBus.subscribe<Neon::KeyPressedEvent>([this](const Neon::KeyPressedEvent &event)
                                                  { this->inputCallback(event); });

        getInput().setCursorMode(Input::CursorMode::Disabled);

        // učitaj mesh
        Ref<Neon::Mesh> mesh = assetManager.load<Neon::Mesh>(
            "Sandbox/assets/skybox.obj");

        // učitaj spojenu 2D skybox teksturu
        Ref<Neon::Texture> texture = assetManager.load<Neon::Texture>(
            "Sandbox/assets/skybox/skybox.png");

        // shader
        Ref<Neon::ShaderStage> vertex = assetManager.load<Neon::ShaderStage>(
            "Sandbox/assets/shaders/skybox.vert");

        Ref<Neon::ShaderStage> fragment = assetManager.load<Neon::ShaderStage>(
            "Sandbox/assets/shaders/skybox.frag");

        Ref<Neon::Shader> shader = assetManager.load(vertex, fragment);

        m_skybox = std::make_shared<Neon::Skybox>(
            mesh,
            shader,
            texture);
    }

    void inputCallback(const Neon::MouseMovedEvent &event)
    {
        if (getInput().getCursorMode() == Input::CursorMode::Normal)
            return;

        const auto &data = event.getData();

        // Pitch (X) - clamp [-90°, +90°]
        m_camera.rotation.x -=
            data.delta.y * getTime().getDeltaTime() * m_rotationSpeed;

        m_camera.rotation.x =
            glm::clamp(
                m_camera.rotation.x,
                -glm::half_pi<float>(),
                glm::half_pi<float>());

        // Yaw (Y) - unlimited
        m_camera.rotation.y -=
            data.delta.x * getTime().getDeltaTime() * m_rotationSpeed;
    }
    void inputCallback(const Neon::KeyPressedEvent &event)
    {
        const auto &data = event.getData();

        if (data.key == Input::KeyAlt)
        {
            const bool disabled =
                getInput().getCursorMode() == Input::CursorMode::Disabled;

            if (disabled)
            {
                // Disabled -> Normal
                // Miš se vidi, kontrole i dalje rade
                getInput().setCursorMode(Input::CursorMode::Normal);
                setGuiEnabled(true);
            }
            else
            {
                // Normal -> Disabled
                // Miš se ponovo capture-uje
                getInput().setCursorMode(Input::CursorMode::Disabled);
                setGuiEnabled(false);
            }

            return;
        }

        if (m_inputLocked)
            return;

        switch (data.key)
        {
        case Input::KeyNumPlus:
            m_moveSpeed += 5.0f;

            if (m_moveSpeed <= 5.0f)
                m_moveSpeed = 5.0f;

            Logging::Info(
                "MoveSpeed: " + std::to_string(m_moveSpeed));
            break;

        case Input::KeyNumMinus:
            m_moveSpeed -= 5.0f;

            if (m_moveSpeed < 5.0f)
                m_moveSpeed = 5.0f;

            Logging::Info(
                "MoveSpeed: " + std::to_string(m_moveSpeed));
            break;

        case Input::KeyNumAsterisk:
            m_rotationSpeed += 0.2f;

            Logging::Info(
                "RotationSpeed: " + std::to_string(m_rotationSpeed));
            break;

        case Input::KeyNumSlash:
            m_rotationSpeed -= 0.2f;

            if (m_rotationSpeed <= 0.0f)
                m_rotationSpeed = 0.0f;

            Logging::Info(
                "RotationSpeed: " + std::to_string(m_rotationSpeed));
            break;

        case Input::KeyF5:
            onSave();
            break;

        case Input::KeyF9:
            Neon::SceneSerializer::load(
                m_scene,
                "Sandbox/assets/scene2.neon",
                getAssetManager());
            break;

        default:
            break;
        }
    }
    void onUpdate(float dt) override
    {
        m_scene.update(dt);

        Input &input = getInput();

        // =====================
        // Movement
        // =====================

        const bool disabled =
            getInput().getCursorMode() == Input::CursorMode::Disabled;

        if (!disabled)
            return;

        if (input.isKeyDown(Input::Key::KeyW))
            m_camera.position += m_camera.getForward() * m_moveSpeed * dt;

        if (input.isKeyDown(Input::Key::KeyS))
            m_camera.position -= m_camera.getForward() * m_moveSpeed * dt;

        if (input.isKeyDown(Input::Key::KeyA))
            m_camera.position -= m_camera.getRight() * m_moveSpeed * dt;

        if (input.isKeyDown(Input::Key::KeyD))
            m_camera.position += m_camera.getRight() * m_moveSpeed * dt;

        if (input.isKeyDown(Input::Key::KeyShift))
            m_camera.position.y += m_moveSpeed * dt;

        if (input.isKeyDown(Input::Key::KeyCtrl))
            m_camera.position.y -= m_moveSpeed * dt;
    }

    void onSave()
    {
        Neon::SceneSerializer::save(m_scene, "Sandbox/assets/scene.neon");
    }

    void onRender() override
    {
        getWindow().setCulingDirection(
            Neon::CulingDirection::Front);

        getWindow().setDepthFunction(
            Neon::DepthFunction::LessEqual);

        m_skybox->render(m_camera, getWindow());

        getWindow().setDepthFunction(
            Neon::DepthFunction::Less);

        getWindow().setCulingDirection(
            Neon::CulingDirection::Back);

        getRenderer().draw(m_scene);
    }

private:
    Ref<Neon::Skybox> m_skybox;

    float m_moveSpeed = 5.0f;
    float m_rotationSpeed = 1; // 90°/s
    bool m_inputLocked = false;

    Neon::Scene m_scene;
    Neon::Camera &m_camera;
};

int main()
{
    Sandbox app = Sandbox();
    app.run();

    return 0;
}