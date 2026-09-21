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
#include <vector>

namespace Neon
{
    class Renderer
    {
        class Skybox;

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

    private:
        // 'lights' is a non-owning view into whichever Scene is being drawn (see
        // Renderer::draw(Scene&)) - Renderer never owns Light data itself, Scene
        // does (Scene::m_lights). When drawing a single GameObject with no Scene
        // context (see the public draw(GameObject&, bool) overload), 'lights' is
        // simply empty and lightCount comes out as 0.
        void drawGameObject(GameObject &gameObject, const std::vector<std::unique_ptr<Light>> &lights);

        Camera &m_camera;
        Window &m_window;
    };
}

#endif