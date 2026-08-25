#include "Renderer.hpp"

#include "Assert.hpp"

namespace Neon
{
    Renderer::Renderer(Camera &cam, Window &win) : m_lights(), m_camera(cam), m_window(win)
    {
    }

    Renderer::~Renderer()
    {
    }

    void Renderer::draw(Mesh &mesh, Material &material, Transform &transform)
    {
        material.bind();

        material.setUniform("u_Model", transform.getMatrix());
        material.setUniform("u_View", m_camera.getViewMatrix());
        material.setUniform("u_Projection", m_camera.getProjectionMatrix(m_window.getAspectRatio()));
        material.setUniform("u_ViewPos", m_camera.position);

        int lightCount = static_cast<int>(m_lights.size());
        material.setUniform("lightCount", lightCount);

        for (int i = 0; i < lightCount; ++i)
        {
            std::string index = std::to_string(i);

            material.setUniform(
                "lights[" + index + "].pos",
                m_lights[i].position);

            material.setUniform(
                "lights[" + index + "].color",
                m_lights[i].color);

            material.setUniform(
                "lights[" + index + "].intensity",
                m_lights[i].intensity);
        }

        mesh.draw();
    }

    void Renderer::addLight(const Light &light)
    {
        m_lights.push_back(light);
    }

    Light &Renderer::getLight(size_t index)
    {
        NEON_ASSERT(
            index < m_lights.size(),
            "Renderer::getLight index out of range");

        return m_lights[index];
    }

    const Light &Renderer::getLight(size_t index) const
    {
        NEON_ASSERT(
            index < m_lights.size(),
            "Renderer::getLight index out of range");

        return m_lights[index];
    }

    void Renderer::clearLights()
    {
        m_lights.clear();
    }
}