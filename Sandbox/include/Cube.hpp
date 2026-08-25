#ifndef SANDBOX_CUBE
#define SANDBOX_CUBE

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

struct Light
{
    glm::vec3 position;
    glm::vec3 color;

    float intensity;
};

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
              {{3, Neon::ShaderDataType::Float},
               {3, Neon::ShaderDataType::Float},
               {2, Neon::ShaderDataType::Float},
               {3, Neon::ShaderDataType::Float}})
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

#endif