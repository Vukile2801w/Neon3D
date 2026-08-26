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

    void Renderer::drawGameObject(GameObject &gameObject)
    {
        NEON_ASSERT(gameObject.material == nullptr, "Passed gameObject without material");
        NEON_ASSERT(gameObject.mesh == nullptr, "Passed gameObject without mesh");

        gameObject.material->bind();

        gameObject.material->setUniform("u_Model", gameObject.getWorldMatrix());
        gameObject.material->setUniform("u_View", m_camera.getViewMatrix());
        gameObject.material->setUniform("u_Projection", m_camera.getProjectionMatrix(m_window.getAspectRatio()));
        gameObject.material->setUniform("u_ViewPos", m_camera.position);

        int lightCount = static_cast<int>(m_lights.size());
        gameObject.material->setUniform("lightCount", lightCount);

        for (int i = 0; i < lightCount; ++i)
        {
            gameObject.material->setUniform("lights", MaterialLight{&m_lights[i], i});
        }

        gameObject.mesh->draw();
    }

    void Renderer::draw(GameObject &gameObject, bool warnIfNotRenderable)
    {
        if (gameObject.mesh && gameObject.material)
            drawGameObject(gameObject);
        else if (warnIfNotRenderable)
            Neon::Logging::Warning("Manualy passed non-renderable GameObject (null mesh or null material)");
    }

    void Renderer::draw(Scene &scene)
    {
        for (const auto &gameObject : scene.getGameObjects())
        {
            if (gameObject)
                draw(*gameObject);
        }
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