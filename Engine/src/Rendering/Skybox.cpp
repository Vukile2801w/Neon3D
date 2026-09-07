#include "Rendering/Skybox.hpp"
#include "Rendering/Window.hpp"

#include "Ref.hpp"

#include "glm.hpp"

namespace Neon
{
    Skybox::Skybox(
        const Ref<Mesh> &mesh,
        const Ref<Shader> &shader,
        const Ref<Texture> &texture)
        : m_mesh(mesh),
          m_shader(shader),
          m_texture(texture)
    {
    }

    void Skybox::render(const Camera &camera, const Window &window)
    {
        m_shader->bind();

        // Uklanjamo translaciju kamere.
        glm::mat4 view =
            glm::mat4(glm::mat3(camera.getViewMatrix()));

        glm::mat4 projection =
            camera.getProjectionMatrix(window.getAspectRatio());

        m_shader->setMat4("u_View", view);
        m_shader->setMat4("u_Projection", projection);

        m_texture->bind(0);
        m_shader->setInt("u_Skybox", 0);

        m_mesh->draw();
    }
}