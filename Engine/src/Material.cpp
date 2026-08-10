#include <variant>

#include "Material.hpp"

namespace Neon
{
    Material::Material(Shader &shader)
        : m_shader(shader)
    {
    }

    void Material::bind() const
    {
        m_shader.bind();

        for (const auto &tex : m_textures)
        {
            tex.texture->bind(tex.slot);
            m_shader.setInt(tex.name, tex.slot);
        }
    }

    void Material::setTexture(const std::string &name, Texture &texture, unsigned int slot)
    {
        m_textures.emplace_back(name, &texture, slot);
    }

    void Material::set(
        const std::string &name,
        MaterialProperty data)
    {
        if (std::holds_alternative<bool>(data))
        {
            m_shader.setBool(name, std::get<bool>(data));
        }
        else if (std::holds_alternative<int>(data))
        {
            m_shader.setInt(name, std::get<int>(data));
        }
        else if (std::holds_alternative<unsigned int>(data))
        {
            m_shader.setUInt(name, std::get<unsigned int>(data));
        }
        else if (std::holds_alternative<float>(data))
        {
            m_shader.setFloat(name, std::get<float>(data));
        }
        else if (std::holds_alternative<glm::vec2>(data))
        {
            const auto &value = std::get<glm::vec2>(data);

            m_shader.setVec2(name,
                             value.x,
                             value.y);
        }
        else if (std::holds_alternative<glm::vec3>(data))
        {
            const auto &value = std::get<glm::vec3>(data);

            m_shader.setVec3(name,
                             value.x,
                             value.y,
                             value.z);
        }
        else if (std::holds_alternative<glm::vec4>(data))
        {
            const auto &value = std::get<glm::vec4>(data);

            m_shader.setVec4(name,
                             value.x,
                             value.y,
                             value.z,
                             value.w);
        }
        else if (std::holds_alternative<glm::mat3>(data))
        {
            const glm::mat3 &matrix = std::get<glm::mat3>(data);

            m_shader.setMat3(name, &matrix[0][0]);
        }
        else if (std::holds_alternative<glm::mat4>(data))
        {
            const glm::mat4 &matrix = std::get<glm::mat4>(data);

            m_shader.setMat4(name, &matrix[0][0]);
        }
    }
}