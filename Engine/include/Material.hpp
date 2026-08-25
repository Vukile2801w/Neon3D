#ifndef NEON_MATERIAL
#define NEON_MATERIAL

#include <vector>
#include <variant>

#include "glm.hpp"

#include "Shader.hpp"
#include "Texture.hpp"
#include "Transform.hpp"
#include "Camera.hpp"

namespace Neon
{
    using MaterialProperty = std::variant<
        bool,
        int,
        unsigned int,
        float,
        glm::vec2,
        glm::vec3,
        glm::vec4,
        glm::mat3,
        glm::mat4>;
    class Material
    {
    public:
        explicit Material(Shader &shader);

        void bind() const;

        void setProperty(
            const std::string &name,
            MaterialProperty value);

        MaterialProperty getProperty(const std::string &name) const;

        void setUniform(
            const std::string &name,
            const MaterialProperty &property) const;

        void setTexture(
            const std::string &name,
            Texture &texture);

    private:
        Shader &m_shader;

        struct TextureSlot
        {
            std::string name;
            Texture *texture;
        };

        std::vector<TextureSlot> m_textures;

        std::unordered_map<std::string, MaterialProperty> m_properties;
    };
}

#endif