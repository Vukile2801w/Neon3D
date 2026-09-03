#include <iterator>
#include <chrono>
#include <sstream>
#include <random>

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
        onSave();
    }

protected:
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

        Neon::SceneSerializer::load(m_scene, "Sandbox/assets/scene.neon", assetManager, getRenderer());

        eventBus.subscribe<Neon::MouseMovedEvent>([this](const Neon::MouseMovedEvent &event)
                                                  { inputCallback(event); });

        eventBus.subscribe<Neon::KeyPressedEvent>([this](const Neon::KeyPressedEvent &event)
                                                  { this->inputCallback(event); });

        eventBus.subscribe<Neon::KeyReleasedEvent>([this](const Neon::KeyReleasedEvent &event)
                                                   {
            if (event.getData().key == Input::KeyAlt)
            getInput().setCursorMode(Input::CursorMode::Disabled); });

        getInput().setCursorMode(Input::CursorMode::Disabled);
    }

    void inputCallback(const Neon::MouseMovedEvent &event)
    {
        if (getInput().getCursorMode() == Input::CursorMode::Normal)
            return;

        const auto &data = event.getData();

        // Pitch (X) - clamp [-90°, +90°]
        m_camera.rotation.x -=
            data.delta.y * getTime().getDeltaTime();

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
        if (getInput().getCursorMode() == Input::CursorMode::Normal)
            return;

        const auto &data = event.getData();

        switch (data.key)
        {
        case Input::KeyAlt:
            getInput().setCursorMode(Input::CursorMode::Normal);
            break;

        case Input::KeyNumPlus:
            m_moveSpeed += 5.0f;
            if (m_moveSpeed <= 5.0f)
                m_moveSpeed = 5.0f;
            Logging::Info("MoveSpeed: " + std::to_string(m_moveSpeed));
            break;

        case Input::KeyNumMinus:
            m_moveSpeed -= 5.0f;

            if (m_moveSpeed <= 5.0f)
                m_moveSpeed = 5.0f;
            Logging::Info("MoveSpeed: " + std::to_string(m_moveSpeed));
            break;

        case Input::KeyNumAsterisk:
            m_rotationSpeed += 0.2f;
            Logging::Info("RotationSpeed: " + std::to_string(m_rotationSpeed));
            break;

        case Input::KeyNumSlash:
            m_rotationSpeed -= 0.2f;

            Logging::Info("RotationSpeed: " + std::to_string(m_rotationSpeed));
            break;

        case Input::KeyF5:
            onSave();
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
        getRenderer().draw(m_scene);
    }

private:
    void spawnCubes(int count)
    {
        const int width =
            static_cast<int>(std::sqrt(count));

        Cube *previousCube = nullptr;
        Cube *previousRowFirst = nullptr;

        for (int i = 0; i < count; ++i)
        {
            const int x = i % width;

            Cube *parent = nullptr;
            glm::vec3 position(0.0f);

            if (i == 0)
            {
                // Prvi Cube nema parenta
                parent = nullptr;
                position = glm::vec3(0.0f);
            }
            else if (x == 0)
            {
                // Prvi Cube novog reda:
                // parent je prvi Cube prethodnog reda
                parent = previousRowFirst;
                position = glm::vec3(0.0f, 0.0f, 2.0f);
            }
            else
            {
                // Ostali Cube-ovi u redu:
                // parent je prethodni Cube
                parent = previousCube;
                position = glm::vec3(2.0f, 0.0f, 0.0f);
            }

            Cube *cube = m_scene.createGameObject<Cube>(
                parent,
                position,
                glm::vec3(1.0f),
                getAssetManager(),
                false);

            // Prvi Cube trenutnog reda
            if (x == 0)
                previousRowFirst = cube;

            // Poslednji Cube
            previousCube = cube;

            if (i % 10 == 0)
            {
                Logging::Warning(
                    "Created Cubes: " +
                    std::to_string(i) +
                    "/" +
                    std::to_string(count));
            }
        }
    }

    Cube *m_lightCube;
    Cube *m_lightCube1;

    float m_moveSpeed = 5.0f;
    float m_rotationSpeed = 1; // 90°/s

    Neon::Scene m_scene;
    Neon::Camera &m_camera;
};

int main()
{
    Sandbox app = Sandbox();
    app.run();

    return 0;
}