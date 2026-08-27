#ifndef NEON_RENDERER
#define NEON_RENDERER

#include "Rendering/Material.hpp"
#include "Rendering/Light.hpp"

#include "Scene/GameObject.hpp"
#include "Scene/Scene.hpp"

#include "Mesh/Mesh.hpp"

#include "Transform.hpp"
#include "Camera.hpp"
#include "Window.hpp"

#include <memory>

namespace Neon
{
    class Renderer
    {
    public:
        Renderer(Camera &cam, Window &win);
        ~Renderer();

        void draw(GameObject &gameObject, bool warnIfNotRenderable = true);
        void draw(Scene &scene);

        Camera &getCamera()
        {
            return m_camera;
        }
        const Camera &getCamera() const { return m_camera; }

        void addLight(const Light &light);
        Light &getLight(size_t index);
        const Light &getLight(size_t index) const;
        void clearLights();

    private:
        void drawGameObject(GameObject &gameObject);

        std::vector<Light> m_lights;

        Camera &m_camera;
        Window &m_window;
    };
}

#endif