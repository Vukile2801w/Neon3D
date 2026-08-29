#include "Rendering/Material.hpp"

#include <variant>

#include "Assert.hpp"

namespace Neon
{
    Material::Material(Ref<Shader> shader)
        : m_shader(std::move(shader))
    {
    }

    void Material::bind() const
    {
        m_shader.get()->bind();

        unsigned int slot = 0;

        for (const auto &tex : m_textures)
        {
            tex.texture->bind(slot);
            m_shader.get()->setInt(tex.name, slot);

            slot++;
        }

        for (const auto &[name, property] : m_properties)
        {
            setUniform(name, property);
        }
    }

    void Material::setTexture(const std::string &name, Ref<Texture> texture)
    {
        m_textures.push_back(TextureSlot{name, std::move(texture)});
    }

    void Material::setProperty(
        const std::string &name,
        MaterialProperty data)
    {
        m_properties[name] = std::move(data);
    }

    MaterialProperty Material::getProperty(const std::string &name) const
    {
        return m_properties.at(name);
    }

    void Material::setUniform(
        const std::string &name,
        const MaterialProperty &property) const
    {
        if (std::holds_alternative<bool>(property))
        {
            m_shader->setBool(name, std::get<bool>(property));
        }
        else if (std::holds_alternative<int>(property))
        {
            m_shader->setInt(name, std::get<int>(property));
        }
        else if (std::holds_alternative<unsigned int>(property))
        {
            m_shader->setUInt(name, std::get<unsigned int>(property));
        }
        else if (std::holds_alternative<float>(property))
        {
            m_shader->setFloat(name, std::get<float>(property));
        }
        else if (std::holds_alternative<glm::vec2>(property))
        {
            const auto &value = std::get<glm::vec2>(property);

            m_shader->setVec2(name,
                              value.x,
                              value.y);
        }
        else if (std::holds_alternative<glm::vec3>(property))
        {
            const auto &value = std::get<glm::vec3>(property);

            m_shader->setVec3(name,
                              value.x,
                              value.y,
                              value.z);
        }
        else if (std::holds_alternative<glm::vec4>(property))
        {
            const auto &value = std::get<glm::vec4>(property);

            m_shader->setVec4(name,
                              value.x,
                              value.y,
                              value.z,
                              value.w);
        }
        else if (std::holds_alternative<glm::mat3>(property))
        {
            const glm::mat3 &matrix = std::get<glm::mat3>(property);

            m_shader->setMat3(name, &matrix[0][0]);
        }
        else if (std::holds_alternative<glm::mat4>(property))
        {
            const glm::mat4 &matrix = std::get<glm::mat4>(property);

            m_shader->setMat4(name, &matrix[0][0]);
        }
        else if (std::holds_alternative<MaterialLight>(property))
        {
            const MaterialLight light = std::get<MaterialLight>(property);

            setUniform(
                name + "[" + std::to_string(light.index) + "].pos",
                light.light->position);

            setUniform(
                name + "[" + std::to_string(light.index) + "].color",
                light.light->color);

            setUniform(
                name + "[" + std::to_string(light.index) + "].intensity",
                light.light->intensity);
        }
        else
        {
            NEON_ASSERT(false, "Unhandled MaterialProperty type in Material::set()");
        }
    }

}