#ifndef NEON_MATERIAL
#define NEON_MATERIAL

#include <vector>
#include <variant>

#include "glm.hpp"

#include "Rendering/Shader.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/Light.hpp"

#include "Transform.hpp"
#include "Camera.hpp"
#include "Ref.hpp"

namespace Neon
{

    struct MaterialLight
    {
        Light *light;
        int index;
    };
    using MaterialProperty = std::variant<
        bool,
        int,
        unsigned int,
        float,
        glm::vec2,
        glm::vec3,
        glm::vec4,
        glm::mat3,
        glm::mat4,
        MaterialLight>;
    class Material
    {
    public:
        struct TextureSlot
        {
            std::string name;
            Ref<Texture> texture;
        };

        explicit Material(Ref<Shader> shader);

        void bind() const;

        void setProperty(
            const std::string &name,
            MaterialProperty value);

        void setUniform(
            const std::string &name,
            const MaterialProperty &property) const;

        void setTexture(
            const std::string &name,
            Ref<Texture> texture);

        const std::unordered_map<std::string, MaterialProperty> &getProperties() const;
        MaterialProperty getProperty(const std::string &name) const;

        const std::vector<TextureSlot> getTextures() const;
        Ref<Shader> getShader() const;

    private:
        Ref<Shader> m_shader;

        std::vector<TextureSlot> m_textures;

        std::unordered_map<std::string, MaterialProperty> m_properties;
    };
}

#endif