#include "Rendering/Renderer.hpp"

#include <string>

#include "Rendering/DirectionalLight.hpp"
#include "Rendering/PointLight.hpp"
#include "Rendering/SpotLight.hpp"
#include "Rendering/Skybox.hpp"
#include "Assert.hpp"

namespace Neon
{
    namespace
    {
        // Must match shader.frag's LIGHT_TYPE_* #defines exactly - this is the one
        // place on the C++ side that knows those numeric values.
        enum class ShaderLightType : int
        {
            Point = 0,
            Directional = 1,
            Spot = 2
        };

        // Writes one Light's uniforms into 'lights[index]' in the shader, dispatching
        // on the concrete subclass exactly once here. To add a new Light subclass:
        // add an "else if" branch below (and a matching LIGHT_TYPE_* + getLightVector()
        // branch in shader.frag) - drawGameObject()'s loop never needs to change.
        void submitLight(Material &material, int index, const Light &light)
        {
            std::string prefix = "lights[" + std::to_string(index) + "]";

            material.setUniform(prefix + ".color", light.color);
            material.setUniform(prefix + ".intensity", light.intensity);

            if (const auto *point = dynamic_cast<const PointLight *>(&light))
            {
                material.setUniform(prefix + ".type", static_cast<int>(ShaderLightType::Point));
                material.setUniform(prefix + ".position", point->position);
                material.setUniform(prefix + ".range", point->range);
            }
            else if (const auto *spot = dynamic_cast<const SpotLight *>(&light))
            {
                material.setUniform(prefix + ".type", static_cast<int>(ShaderLightType::Spot));
                material.setUniform(prefix + ".position", spot->position);
                material.setUniform(prefix + ".direction", spot->direction);
                material.setUniform(prefix + ".range", spot->range);
                material.setUniform(prefix + ".innerConeDeg", spot->innerCone);
                material.setUniform(prefix + ".outerConeDeg", spot->outerCone);
            }
            else if (const auto *directional = dynamic_cast<const DirectionalLight *>(&light))
            {
                material.setUniform(prefix + ".type", static_cast<int>(ShaderLightType::Directional));
                material.setUniform(prefix + ".direction", directional->direction);
            }
            else
            {
                // A new Light subclass was added without adding a branch above -
                // this is a programmer error (missing shader wiring), not a runtime
                // failure, so it's an assert rather than a Logging::Error (see
                // CONTRIBUTING.md §5/§6).
                NEON_ASSERT(false, "Renderer::submitLight - unrecognized Light subtype, add a branch for it");
            }
        }
    }

    Renderer::Renderer(Camera &cam, Window &win) : m_camera(cam), m_window(win)
    {
    }

    Renderer::~Renderer()
    {
    }

    void Renderer::drawGameObject(GameObject &gameObject, const std::vector<std::unique_ptr<Light>> &lights)
    {
        NEON_ASSERT(gameObject.material != nullptr, "Passed gameObject without material");
        NEON_ASSERT(gameObject.mesh != nullptr, "Passed gameObject without mesh");

        gameObject.material->bind();

        gameObject.material->setUniform("u_Model", gameObject.getWorldMatrix());
        gameObject.material->setUniform("u_View", m_camera.getViewMatrix());
        gameObject.material->setUniform("u_Projection", m_camera.getProjectionMatrix(m_window.getAspectRatio()));
        gameObject.material->setUniform("u_ViewPos", m_camera.position);

        // Every light type (Point/Spot/Directional) shares the same shader-side
        // 'lights[]' array (see shader.frag) - submitLight() picks the right fields
        // per concrete type, so this loop stays the same no matter how many light
        // types exist.
        int lightCount = 0;

        for (const auto &light : lights)
        {
            if (lightCount >= 32) // matches shader.frag's MAX_LIGHTS
                break;

            submitLight(*gameObject.material, lightCount, *light);
            ++lightCount;
        }

        gameObject.material->setUniform("lightCount", lightCount);

        gameObject.mesh->draw();
    }

    void Renderer::draw(GameObject &gameObject, bool warnIfNotRenderable)
    {
        static const std::vector<std::unique_ptr<Light>> noLights;

        if (gameObject.mesh && gameObject.material)
            drawGameObject(gameObject, noLights);
        else if (warnIfNotRenderable)
            Neon::Logging::Warning("Manualy passed non-renderable GameObject (null mesh or null material)");
    }

    void Renderer::draw(Scene &scene)
    {
        const std::vector<std::unique_ptr<Light>> &lights = scene.getLights();

        for (const auto &gameObject : scene.getGameObjects())
        {
            if (gameObject && gameObject->mesh && gameObject->material)
                drawGameObject(*gameObject, lights);
            else if (gameObject)
                Neon::Logging::Warning("Manualy passed non-renderable GameObject (null mesh or null material)");
        }
    }
}