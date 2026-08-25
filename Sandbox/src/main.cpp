#include <iterator>
#include <chrono>
#include <sstream>

#include "Cube.hpp"

#include "gtc/matrix_transform.hpp"

#include "Events/EventBus.hpp"
#include "Logging.hpp"
#include "Material.hpp"
#include "Window.hpp"
#include "Shader.hpp"
#include "Mesh/Mesh.hpp"
#include "Texture.hpp"
#include "Transform.hpp"
#include "Camera.hpp"
#include "Time.hpp"
#include "Input.hpp"
#include "Application.hpp"
#include "Events/KeyPressedEvent.hpp"

// Enums
using Neon::EventBus;
using Neon::Input;
using Neon::Logging;
using Neon::ShaderDataType;
using Neon::TextureFilter;

class Sandbox : public Neon::Application
{
public:
    Sandbox() {}

protected:
    void onStart() override
    {
        m_camera.position.y = 1.3f;
        m_camera.FOV = 60;

        m_cubes.reserve(2);

        m_cubes.emplace_back(
            glm::vec3(0.0f, 0.0f, -1.0f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            false);
        m_cubes[0].setColor({0.3f, 0.0f, 1.0f});

        m_cubes.emplace_back(
            glm::vec3(1.0f, 1.0f, -1.0f),
            glm::vec3(0.3f, 0.3f, 0.3f),
            true);
        m_cubes[1].setColor({1.0f, 1.0f, 0.0f});

        m_lights.push_back(
            {m_cubes[0].transform.position,
             {1.0f, 1.0f, 1.0f},
             1.0f});
        m_lights.push_back(
            {m_cubes[1].transform.position,
             {1.0f, 1.0f, 0.0f},
             10.0f});
    }

    void onUpdate(float dt) override
    {
        float moveSpeed = 5.0f;
        float rotationSpeed = glm::radians(40.0f); // 90°/s

        Input &input = getInput();
        EventBus &eventBus = getEventBus();

        // =====================
        // Movement
        // =====================

        eventBus.subscribe<Neon::KeyPressedEvent>(
            [](const Neon::KeyPressedEvent &event)
            {
                Input::Key key =
                    static_cast<Input::Key>(event.getData().key);

                std::ostringstream msg;
                msg << "Key Pressed: " << key;

                // Logging::Info(msg.str());
            });

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
            m_camera.rotation = {m_camera.rotation.x, m_camera.rotation.y, 0.0f};

        // =====================
        // m_camera roll
        // =====================

        if (input.isKeyDown(Input::Key::KeyQ))
            m_camera.rotation.z += rotationSpeed * dt;

        if (input.isKeyDown(Input::Key::KeyE))
            m_camera.rotation.z -= rotationSpeed * dt;

        float t = static_cast<float>(getTime().sinceStart());

        float radius = 20.0f;

        float angle = t * 1.5f;

        glm::vec3 orbit(
            glm::cos(angle) * radius,
            0.0f,
            glm::sin(angle) * radius);

        glm::mat4 rotation(1.0f);

        rotation = glm::rotate(
            rotation,
            t * 0.4f,
            glm::vec3(1.0f, 0.0f, 0.0f));

        rotation = glm::rotate(
            rotation,
            t * 0.3f,
            glm::vec3(0.0f, 1.0f, 0.0f));

        rotation = glm::rotate(
            rotation,
            t * 0.2f,
            glm::vec3(0.0f, 0.0f, 1.0f));

        orbit =
            glm::vec3(rotation * glm::vec4(orbit, 1.0f));

        m_cubes[1].transform.position =
            m_cubes[0].transform.position + orbit;

        m_lights[1].position =
            m_cubes[1].transform.position;
    }

    void onRender() override
    {
        for (auto &cube : m_cubes)
        {
            cube.render(
                m_camera,
                getWindow().getAspectRatio(),
                m_lights.data(),
                m_lights.size());
        }
    }

private:
    std::vector<Cube> m_cubes;
    std::vector<Light> m_lights;

    Neon::Camera m_camera;
};

int main()
{
    Sandbox app = Sandbox();
    app.run();

    return 0;
}