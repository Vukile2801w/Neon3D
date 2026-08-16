#ifndef NEON_MATERIAL
#define NEON_MATERIAL

#include <vector>
#include <variant>

#include "glm.hpp"

#include "Shader.hpp"
#include "Texture.hpp"

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
        Material(Shader &shader);

        void bind() const;

        void setTexture(
            const std::string &name,
            Texture &texture,
            unsigned int slot);

        void set(const std::string &name, MaterialProperty data);

        glm::vec3 color;

    private:
        Shader &m_shader;

        struct TextureSlot
        {
            std::string name;
            Texture *texture;
            unsigned int slot;
        };

        std::vector<TextureSlot> m_textures;
    };
}

#endif