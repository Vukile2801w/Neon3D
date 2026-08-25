#ifndef NEON_RENDERER
#define NEON_RENDERER

#include "Mesh/Mesh.hpp"
#include "Material.hpp"
#include "Transform.hpp"
#include "Camera.hpp"
#include "Window.hpp"
#include "Light.hpp"

#include <memory>

namespace Neon
{
    class Renderer
    {
    public:
        Renderer(Camera &cam, Window &win);
        ~Renderer();

        void draw(Mesh &mesh, Material &material, Transform &transform);

        Camera &getCamera() { return m_camera; }
        const Camera &getCamera() const { return m_camera; }

        void addLight(const Light &light);
        Light &getLight(size_t index);
        const Light &getLight(size_t index) const;
        void clearLights();

    private:
        std::vector<Light> m_lights;

        Camera &m_camera;
        Window &m_window;
    };
}

#endif