#ifndef SANDBOX_CUBE
#define SANDBOX_CUBE

#include "Neon.hpp"
#include "json.hpp"

using Neon::Ref;

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

        return assetManager.load(vert, frag);
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
          m_texture(
              assetManager.load<Neon::Texture>(
                  "Sandbox\\assets\\teapot.png")),
          m_normalMap(
              assetManager.load<Neon::Texture>(
                  "Sandbox\\assets\\teapot.png"))
    {
        transform.position = pos;
        transform.scale = scale;

        material = &m_mat;

        m_mat.setTexture("T_Color", m_texture);
        m_mat.setTexture("T_Normal", m_normalMap);

        m_mat.setProperty("u_Shininess", 16.0f);
        m_mat.setProperty("u_IsLightSource", isLightSource);
    }

    std::string getTypeName() override
    {
        return "Cube";
    }

    void setColor(glm::vec3 color)
    {
        m_mat.setProperty("u_Color", color);
    }

    static Neon::GameObject *create(
        Neon::Scene &scene,
        const nlohmann::json &node,
        Neon::AssetManager &assetManager)
    {
        return scene.createGameObject<Cube>(
            nullptr,
            glm::vec3(0.0f),
            glm::vec3(1.0f),
            assetManager,
            false);
    }

private:
    Ref<Neon::Shader> m_shader;
    Neon::Material m_mat;

    Ref<Neon::Texture> m_texture;
    Ref<Neon::Texture> m_normalMap;
};

#endif