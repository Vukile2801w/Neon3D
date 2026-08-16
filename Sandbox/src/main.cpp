#include <iterator>
#include <chrono>

#include "Logging.hpp"
#include "Material.hpp"
#include "Window.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"
#include "Texture.hpp"
#include "Transform.hpp"
#include "Camera.hpp"

// Enums
using Neon::Logging;
using Neon::ShaderDataType;
using Neon::TextureFilter;

float getTime()
{
    using namespace std::chrono;

    static const auto start = high_resolution_clock::now();

    auto now = high_resolution_clock::now();

    return duration<float>(now - start).count();
}
float cubeVertices[] = {
    // Front (+Z)
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

    // Back (-Z)
    0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

    // Left (-X)
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,

    // Right (+X)
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

    // Top (+Y)
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,

    // Bottom (-Y)
    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
    0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f};
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
};

class Cube
{
public:
    Cube(glm::vec3 pos, glm::vec3 scale)
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
               {3, ShaderDataType::Float}})
    {
        transform.position = pos;
        transform.scale = scale;
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
        }

        m_mesh.draw();
    }

    void setColor(glm::vec3 color)
    {
        m_mat.color = color;
    }

    Neon::Transform transform;

private:
    Neon::Shader m_shader;
    Neon::Material m_mat;
    Neon::Mesh m_mesh;
};

int main()
{
    std::unique_ptr<Neon::Window> window =
        std::make_unique<Neon::Window>(); // Kreiranje prozora

    Neon::Camera camera;
    camera.position.y = 1.3f;
    camera.rotation.x = glm::radians(-20.0f);
    camera.FOV = 60;

    Cube cube = Cube({0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 1.0f});
    cube.setColor({0.0f, 1.0f, 0.0f});

    Cube light = Cube({1.0f, 1.0f, 1.0f}, {0.3f, 0.3f, 0.3f});
    light.setColor({1.0f, 0.0f, 0.0f});
    Cube light1 = Cube({-1.0f, 1.0f, 1.0f}, {0.3f, 0.3f, 0.3f});
    light1.setColor({0.0f, 1.0f, 0.0f});

    Light lights[] = {{{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}}, {{-1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}}};

    float cTime = getTime();
    float lTime;
    while (!window->shoudWindowsClose())
    {
        lTime = cTime;
        cTime = getTime();

        cube.transform.rotation.y += cTime - lTime;

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
        light1.render(
            camera,
            window->getAspectRatio(),
            lights,
            sizeof(lights) / sizeof(lights[0]));

        window->render(); // Osvezavanje ekrana
    }

    return 0;
}