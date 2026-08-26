#ifndef SANDBOX_CUBE
#define SANDBOX_CUBE

#include "Scene/GameObject.hpp"
#include "Scene/Behavior.hpp"

#include "Application.hpp"
#include "Logging.hpp"
#include "Material.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Mesh/Mesh.hpp"

struct Light
{
    glm::vec3 position;
    glm::vec3 color;
    float intensity;
};

float cubeVertices[] = {
    // Front (+Z)
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
    0, 1, 2,
    2, 3, 0,

    4, 5, 6,
    6, 7, 4,

    8, 9, 10,
    10, 11, 8,

    12, 13, 14,
    14, 15, 12,

    16, 17, 18,
    18, 19, 16,

    20, 21, 22,
    22, 23, 20};

class CubeBehavior : public Neon::Behavior
{
public:
    CubeBehavior(
        Neon::GameObject *gameObject,
        Neon::Scene *scene,
        Neon::Application *application)
        : Neon::Behavior(gameObject, scene, application),
          m_startPosition(gameObject->transform.position)
    {
    }

    void update(float dt) override
    {
        float t = m_application->getTime().sinceStart();

        m_gameObject->transform.position =
            m_startPosition +
            glm::vec3(
                0.0f,
                std::sin(t),
                0.0f);
    }

private:
    glm::vec3 m_startPosition;
};

class Cube : public Neon::GameObject
{

public:
    Cube(
        Neon::Scene *scene,
        GameObject *parent,
        glm::vec3 pos,
        glm::vec3 scale,
        bool isLightSource)
        : Neon::GameObject(scene, parent),
          m_shader(
              "Sandbox\\assets\\shaders\\shader.vert",
              "Sandbox\\assets\\shaders\\shader.frag"),
          m_mat(m_shader),
          m_texture("Sandbox\\assets\\brick.jpg"),
          m_normalMap("Sandbox\\assets\\brickNormal.png")
    {
        transform.position = pos;
        transform.scale = scale;

        mesh = &getCubeMesh();
        material = &m_mat;

        m_mat.setTexture("T_Color", m_texture);
        m_mat.setTexture("T_Normal", m_normalMap);

        m_mat.setProperty("u_Shininess", 16.0f);
        m_mat.setProperty("u_IsLightSource", isLightSource);

        if (!isLightSource)
            setBehavior<CubeBehavior>();
    }

    void setColor(glm::vec3 color)
    {
        m_mat.setProperty("u_Color", color);
    }

private:
    Neon::Shader m_shader;
    Neon::Material m_mat;

    Neon::Texture m_texture;
    Neon::Texture m_normalMap;

    static Neon::Mesh &getCubeMesh()
    {
        static Neon::Mesh mesh(
            cubeVertices,
            sizeof(cubeVertices),
            cubeIndices,
            sizeof(cubeIndices),
            {{3, Neon::ShaderDataType::Float},
             {3, Neon::ShaderDataType::Float},
             {2, Neon::ShaderDataType::Float},
             {3, Neon::ShaderDataType::Float}});

        return mesh;
    }
};

#endif