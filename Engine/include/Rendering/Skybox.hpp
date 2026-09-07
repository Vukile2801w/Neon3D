#ifndef NEON_SKYBOX_HPP
#define NEON_SKYBOX_HPP

#include <memory>

#include "Mesh/Mesh.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Camera.hpp"
#include "Ref.hpp"
#include "Window.hpp"

namespace Neon
{
    class Skybox
    {
    public:
        Skybox(
            const Ref<Mesh> &mesh,
            const Ref<Shader> &shader,
            const Ref<Texture> &texture);

        void render(const Camera &camer, const Window &window);

    private:
        Ref<Mesh> m_mesh;
        Ref<Shader> m_shader;
        Ref<Texture> m_texture;
    };
}

#endif