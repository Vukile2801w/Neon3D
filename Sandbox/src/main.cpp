#include <iterator>
#include <chrono>

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

// Enums
using Neon::Input;
using Neon::Logging;
using Neon::ShaderDataType;
using Neon::TextureFilter;

float cubeVertices[] = {
    // Front (+Z)
    // Position          Normal            UV          Tangent
    -0.5f, -0.5f, 0.5f, 0, 0, 1, 0, 0, 1, 0, 0,
    0.5f, -0.5f, 0.5f, 0, 0, 1, 1, 0, 1, 0, 0,
    0.5f, 0.5f, 0.5f, 0, 0, 1, 1, 1, 1, 0, 0,
    -0.5f, 0.5f, 0.5f, 0, 0, 1, 0, 1, 1, 0, 0,

    // Back (-Z)
    -0.5f, -0.5f, -0.5f, 0, 0, -1, 1, 0, -1, 0, 0,
    -0.5f, 0.5f, -0.5f, 0, 0, -1, 1, 1, -1, 0, 0,
    0.5f, 0.5f, -0.5f, 0, 0, -1, 0, 1, -1, 0, 0,
    0.5f, -0.5f, -0.5f, 0, 0, -1, 0, 0, -1, 0, 0,

    // Left (-X)
    -0.5f, -0.5f, -0.5f, -1, 0, 0, 0, 0, 0, 0, 1,
    -0.5f, -0.5f, 0.5f, -1, 0, 0, 1, 0, 0, 0, 1,
    -0.5f, 0.5f, 0.5f, -1, 0, 0, 1, 1, 0, 0, 1,
    -0.5f, 0.5f, -0.5f, -1, 0, 0, 0, 1, 0, 0, 1,

    // Right (+X)
    0.5f, -0.5f, 0.5f, 1, 0, 0, 0, 0, 0, 0, -1,
    0.5f, -0.5f, -0.5f, 1, 0, 0, 1, 0, 0, 0, -1,
    0.5f, 0.5f, -0.5f, 1, 0, 0, 1, 1, 0, 0, -1,
    0.5f, 0.5f, 0.5f, 1, 0, 0, 0, 1, 0, 0, -1,

    // Top (+Y)
    -0.5f, 0.5f, 0.5f, 0, 1, 0, 0, 0, 1, 0, 0,
    0.5f, 0.5f, 0.5f, 0, 1, 0, 1, 0, 1, 0, 0,
    0.5f, 0.5f, -0.5f, 0, 1, 0, 1, 1, 1, 0, 0,
    -0.5f, 0.5f, -0.5f, 0, 1, 0, 0, 1, 1, 0, 0,

    // Bottom (-Y)
    -0.5f, -0.5f, -0.5f, 0, -1, 0, 0, 0, 1, 0, 0,
    0.5f, -0.5f, -0.5f, 0, -1, 0, 1, 0, 1, 0, 0,
    0.5f, -0.5f, 0.5f, 0, -1, 0, 1, 1, 1, 0, 0,
    -0.5f, -0.5f, 0.5f, 0, -1, 0, 0, 1, 1, 0, 0};
unsigned int cubeIndices[] = {
    // Front
    0, 1, 2,
    2, 3, 0,

    // Back
    4, 5, 6,
    6, 7, 4,

    // Left
    8, 9, 10,
    10, 11, 8,

    // Right
    12, 13, 14,
    14, 15, 12,

    // Top
    16, 17, 18,
    18, 19, 16,

    // Bottom
    20, 21, 22,
    22, 23, 20};
struct Light
{
    glm::vec3 position;
    glm::vec3 color;

    float intensity;
};

class Cube
{
public:
    Cube(glm::vec3 pos, glm::vec3 scale, bool isLightSource)
        : m_shader(
              "C:/Users/wukbg/programing/C++/Neon3D/Sandbox/shaders/shader.vert",
              "C:/Users/wukbg/programing/C++/Neon3D/Sandbox/shaders/shader.frag"),
          m_mat(m_shader),
          m_mesh(
              cubeVertices,
              sizeof(cubeVertices),
              cubeIndices,
              sizeof(cubeIndices),
              {{3, ShaderDataType::Float},
               {3, ShaderDataType::Float},
               {2, ShaderDataType::Float},
               {3, ShaderDataType::Float}})
    {
        transform.position = pos;
        transform.scale = scale;

        m_isLightSource = isLightSource;

        m_texture =
            new Neon::Texture("Sandbox/assets/brick.jpg");
        m_normalMap =
            new Neon::Texture("Sandbox/assets/brickNormal.png");

        m_mat.setTexture("T_Color", *m_texture);
        m_mat.setTexture("T_Normal", *m_normalMap);
    }

    ~Cube()
    {
        delete m_texture;
        delete m_normalMap;
    }

    void render(
        const Neon::Camera &camera,
        float aspect,
        const Light *lights,
        int lightCount)
    {
        m_mat.bind();

        m_mat.set("u_Model", transform.getMatrix());
        m_mat.set("u_View", camera.getViewMatrix());
        m_mat.set("u_Projection", camera.getProjectionMatrix(aspect));

        m_mat.set("u_Color", m_mat.color);

        m_mat.set("u_ViewPos", camera.position);
        m_mat.set("u_Shininess", 4.0f);
        m_mat.set("u_IsLightSource", m_isLightSource);

        m_mat.set("lightCount", lightCount);

        for (int i = 0; i < lightCount; ++i)
        {
            std::string index = std::to_string(i);

            m_mat.set(
                "lights[" + index + "].pos",
                lights[i].position);

            m_mat.set(
                "lights[" + index + "].color",
                lights[i].color);

            m_mat.set(
                "lights[" + index + "].intensity",
                lights[i].intensity);
        }

        m_mesh.draw();
    }

    void setColor(glm::vec3 color)
    {
        m_mat.color = color;
    }

    Neon::Transform transform;

private:
    bool m_isLightSource;
    Neon::Texture *m_normalMap;
    Neon::Texture *m_texture;
    Neon::Shader m_shader;
    Neon::Material m_mat;
    Neon::Mesh m_mesh;
};

int main()
{
    std::unique_ptr<Neon::Window> window =
        std::make_unique<Neon::Window>(); // Kreiranje prozora

    Neon::Input input = Neon::Input(window.get());

    auto time = Neon::Time();

    Neon::Camera camera;
    camera.position.y = 1.3f;
    camera.FOV = 60;

    Cube cube = Cube({0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 1.0f}, false);
    cube.setColor({0.3f, 0.0f, 1.0f});

    Cube light = Cube({1.0f, 1.0f, -1.0f}, {0.3f, 0.3f, 0.3f}, true);
    light.setColor({1.0f, 1.0f, 0.0f});

    Light lights[] = {
        {light.transform.position,
         {1.0f, 1.0f, 0.0f},
         10.0f},
        {cube.transform.position,
         {1.0f, 1.0f, 1.0f},
         1.0f},
    };

    while (!window->shouldWindowsClose())
    {
        time.beginFrame();
        input.handleInput();

        float dt = time.getDeltaTime();

        float moveSpeed = 5.0f;
        float rotationSpeed = glm::radians(40.0f); // 90°/s

        // =====================
        // Movement
        // =====================

        if (input.isKeyDown(Input::Key::KEY_W))
            camera.position += camera.getForward() * moveSpeed * dt;

        if (input.isKeyDown(Input::Key::KEY_S))
            camera.position -= camera.getForward() * moveSpeed * dt;

        if (input.isKeyDown(Input::Key::KEY_A))
            camera.position -= camera.getRight() * moveSpeed * dt;

        if (input.isKeyDown(Input::Key::KEY_D))
            camera.position += camera.getRight() * moveSpeed * dt;

        if (input.isKeyDown(Input::Key::KEY_SHIFT))
            camera.position.y += moveSpeed * dt;

        if (input.isKeyDown(Input::Key::KEY_CTRL))
            camera.position.y -= moveSpeed * dt;

        // =====================
        // Camera rotation
        // =====================

        if (input.isKeyDown(Input::Key::KEY_UP))
            camera.rotation.x += rotationSpeed * dt;

        if (input.isKeyDown(Input::Key::KEY_DOWN))
            camera.rotation.x -= rotationSpeed * dt;

        if (input.isKeyDown(Input::Key::KEY_LEFT))
            camera.rotation.y += rotationSpeed * dt;

        if (input.isKeyDown(Input::Key::KEY_RIGHT))
            camera.rotation.y -= rotationSpeed * dt;

        if (input.isKeyDown(Input::Key::KEY_ESCAPE))
            camera.rotation = {0.0f, 0.0f, 0.0f};

        // =====================
        // Camera roll
        // =====================

        if (input.isKeyDown(Input::Key::KEY_Q))
            camera.rotation.z += rotationSpeed * dt;

        if (input.isKeyDown(Input::Key::KEY_E))
            camera.rotation.z -= rotationSpeed * dt;

        float t = static_cast<float>(time.sinceStart());

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

        light.transform.position =
            cube.transform.position + orbit;

        lights[0].position =
            light.transform.position;

        cube.render(
            camera,
            window->getAspectRatio(),
            lights,
            sizeof(lights) / sizeof(lights[0]));

        light.render(
            camera,
            window->getAspectRatio(),
            lights,
            sizeof(lights) / sizeof(lights[0]));

        window->render(); // Osvezavanje ekrana
    }
    return 0;
}