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

protected:
    void onStart() override
    {
        EventBus &eventBus = getEventBus();
        AssetManager &assetManager = getAssetManager();

        m_camera.position.y = 1.3f;
        m_camera.FOV = 60;

        spawnCubes(121);

        m_lightCube = m_scene.createGameObject<Cube>(
            nullptr,
            glm::vec3(-5.0f, 10.0f, -1.0f),
            glm::vec3(0.3f, 0.3f, 0.3f),
            assetManager,
            true);
        m_lightCube->setColor({1.0f, 1.0f, 0.0f});

        getRenderer().addLight(
            {m_lightCube->transform.position,
             {1.0f, 1.0f, 1.0f},
             3.0f});
        eventBus.subscribe<Neon::MouseMovedEvent>([this](const Neon::MouseMovedEvent &event)
                                                  { inputCallback(event); });

        eventBus.subscribe<Neon::KeyPressedEvent>([this](const Neon::KeyPressedEvent &event)
                                                  {
            if (event.getData().key == Input::KeyAlt)
            getInput().setCursorMode(Input::CursorMode::Normal); });

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
            data.delta.x * getTime().getDeltaTime();
    }

    void onUpdate(float dt) override
    {
        m_scene.update(dt);

        float moveSpeed = 5.0f;
        float rotationSpeed = glm::radians(90.0f); // 90°/s

        Input &input = getInput();

        // =====================
        // Movement
        // =====================

        if (input.isKeyDown(Input::Key::KeyW))
            m_camera.position += m_camera.getForward() * moveSpeed * dt;

        if (input.isKeyDown(Input::Key::KeyS))
            m_camera.position -= m_camera.getForward() * moveSpeed * dt;

        if (input.isKeyDown(Input::Key::KeyA))
            m_camera.position -= m_camera.getRight() * moveSpeed * dt;

        if (input.isKeyDown(Input::Key::KeyD))
            m_camera.position += m_camera.getRight() * moveSpeed * dt;

        if (input.isKeyDown(Input::Key::KeyShift))
            m_camera.position.y += moveSpeed * dt;

        if (input.isKeyDown(Input::Key::KeyCtrl))
            m_camera.position.y -= moveSpeed * dt;

        // =====================
        // m_camera rotation
        // =====================

        if (input.isKeyDown(Input::Key::KeyUp))
            m_camera.rotation.x += rotationSpeed * dt;

        if (input.isKeyDown(Input::Key::KeyDown))
            m_camera.rotation.x -= rotationSpeed * dt;

        if (input.isKeyDown(Input::Key::KeyLeft))
            m_camera.rotation.y += rotationSpeed * dt;

        if (input.isKeyDown(Input::Key::KeyRight))
            m_camera.rotation.y -= rotationSpeed * dt;

        if (input.isKeyDown(Input::Key::KeyEscape))
            m_camera.rotation = {0.0f, 0.0f, 0.0f};

        // =====================
        // m_camera roll
        // =====================

        if (input.isKeyDown(Input::Key::KeyQ))
            m_camera.rotation.z += rotationSpeed * dt;

        if (input.isKeyDown(Input::Key::KeyE))
            m_camera.rotation.z -= rotationSpeed * dt;
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

    Neon::Scene m_scene;
    Neon::Camera &m_camera;
};

int main()
{
    Sandbox app = Sandbox();
    app.run();

    return 0;
}