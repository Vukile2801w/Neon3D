#include <iterator>
#include <chrono>

#include "Cube.hpp"

#include "gtc/matrix_transform.hpp"

#include "Logging.hpp"
#include "Material.hpp"
#include "Window.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"
#include "Texture.hpp"
#include "Transform.hpp"
#include "Camera.hpp"
#include "Time.hpp"
#include "Input.hpp"
#include "Application.hpp"

// Enums
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

        // =====================
        // Movement
        // =====================

        if (getInput().isKeyDown(Input::Key::KEY_W))
            m_camera.position += m_camera.getForward() * moveSpeed * dt;

        if (getInput().isKeyDown(Input::Key::KEY_S))
            m_camera.position -= m_camera.getForward() * moveSpeed * dt;

        if (getInput().isKeyDown(Input::Key::KEY_A))
            m_camera.position -= m_camera.getRight() * moveSpeed * dt;

        if (getInput().isKeyDown(Input::Key::KEY_D))
            m_camera.position += m_camera.getRight() * moveSpeed * dt;

        if (getInput().isKeyDown(Input::Key::KEY_SHIFT))
            m_camera.position.y += moveSpeed * dt;

        if (getInput().isKeyDown(Input::Key::KEY_CTRL))
            m_camera.position.y -= moveSpeed * dt;

        // =====================
        // m_camera rotation
        // =====================

        if (getInput().isKeyDown(Input::Key::KEY_UP))
            m_camera.rotation.x += rotationSpeed * dt;

        if (getInput().isKeyDown(Input::Key::KEY_DOWN))
            m_camera.rotation.x -= rotationSpeed * dt;

        if (getInput().isKeyDown(Input::Key::KEY_LEFT))
            m_camera.rotation.y += rotationSpeed * dt;

        if (getInput().isKeyDown(Input::Key::KEY_RIGHT))
            m_camera.rotation.y -= rotationSpeed * dt;

        if (getInput().isKeyDown(Input::Key::KEY_ESCAPE))
            m_camera.rotation = {m_camera.rotation.x, m_camera.rotation.y, 0.0f};

        // =====================
        // m_camera roll
        // =====================

        if (getInput().isKeyDown(Input::Key::KEY_Q))
            m_camera.rotation.z += rotationSpeed * dt;

        if (getInput().isKeyDown(Input::Key::KEY_E))
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