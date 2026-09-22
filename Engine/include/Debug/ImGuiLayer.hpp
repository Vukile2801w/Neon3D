#ifndef NEON_IMGUILAYER
#define NEON_IMGUILAYER

namespace Neon
{
    class Application;
    class Scene;
    class GameObject;
    class Material;
    class Light;

    class ImGuiLayer
    {
    public:
        ImGuiLayer(Application *application);
        ~ImGuiLayer();

        void attachScene(Scene *scene);

        void begin();
        void end();

    private:
        void drawStatsPanel();
        void drawSceneHierarchyPanel();
        void drawInspectorPanel(GameObject *object);
        void drawMaterialSection(Material *material);
        void drawLightsPanel();

        Application *m_application = nullptr;
        Scene *m_scene = nullptr;
        GameObject *m_selectedObject = nullptr;
    };
}

#endif