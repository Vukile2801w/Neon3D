#ifndef SANDBOX_CUBE
#define SANDBOX_CUBE

#include "Neon.hpp"

using Neon::Ref;

#include <memory>

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
    Ref<Neon::Shader> helper(Neon::AssetManager &assetManager)
    {
        Ref<Neon::ShaderStage> vert =
            assetManager.load<Neon::ShaderStage>(
                "Sandbox/assets/shaders/shader.vert");

        Ref<Neon::ShaderStage> frag =
            assetManager.load<Neon::ShaderStage>(
                "Sandbox/assets/shaders/shader.frag");

        Ref<Neon::Shader> shader =
            assetManager.load(vert, frag);

        return shader;
    }

    Cube(
        Neon::Scene *scene,
        GameObject *parent,
        glm::vec3 pos,
        glm::vec3 scale,
        Neon::AssetManager &assetManager,
        bool isLightSource)
        : Neon::GameObject(scene, parent),
          m_shader(helper(assetManager)),
          m_mat(m_shader),
          m_texture(assetManager.load<Neon::Texture>("Sandbox\\assets\\brick.jpg")),
          m_normalMap(assetManager.load<Neon::Texture>("Sandbox\\assets\\brickNormal.png"))
    {

        transform.position = pos;
        transform.scale = scale;

        mesh = assetManager.load<Neon::Mesh>("Sandbox\\assets\\cube.obj");

        material = &m_mat;

        m_mat.setTexture("T_Color", m_texture);
        m_mat.setTexture("T_Normal", m_normalMap);

        m_mat.setProperty("u_Shininess", 16.0f);
        m_mat.setProperty("u_IsLightSource", isLightSource);

        if (!isLightSource)
            setBehavior<CubeBehavior>();
    }

    std::string getTypeName() override
    {
        return "Cube";
    }

    void setColor(glm::vec3 color)
    {
        m_mat.setProperty("u_Color", color);
    }

private:
    Ref<Neon::Shader> m_shader;
    Neon::Material m_mat;

    Ref<Neon::Texture> m_texture;
    Ref<Neon::Texture> m_normalMap;
};

#endif