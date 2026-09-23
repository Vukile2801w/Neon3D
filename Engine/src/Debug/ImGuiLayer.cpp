#include "Debug/ImGuiLayer.hpp"

#include <cstdint>
#include <variant>

#include "Application.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/DirectionalLight.hpp"
#include "Rendering/Light.hpp"
#include "Rendering/PointLight.hpp"
#include "Rendering/SpotLight.hpp"
#include "Rendering/Window.hpp"
#include "Scene/GameObject.hpp"
#include "Scene/Scene.hpp"
#include "Transform.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace Neon
{
    ImGuiLayer::ImGuiLayer(Application *application)
    {
        m_application = application;

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

        // Setup Platform/Renderer backends
        m_application->getWindow().InitImGUI();
    }

    ImGuiLayer::~ImGuiLayer()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiLayer::attachScene(Scene *scene)
    {
        m_scene = scene;
        m_selectedObject = nullptr;
    }

    void ImGuiLayer::begin()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Debug");

        drawStatsPanel();
        ImGui::Separator();
        drawSceneHierarchyPanel();
        ImGui::Separator();
        drawInspectorPanel(m_selectedObject);
        ImGui::Separator();
        drawLightsPanel();

        ImGui::End();
    }

    void ImGuiLayer::end()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void ImGuiLayer::drawStatsPanel()
    {
        if (!ImGui::CollapsingHeader("Stats", ImGuiTreeNodeFlags_DefaultOpen))
            return;

        const float fps = m_application->getTime().getFPS();
        const float frameMs = m_application->getTime().getDeltaTime() * 1000.0f;

        ImGui::Text("FPS: %.1f", fps);
        ImGui::Text("Frame: %.2f ms", frameMs);

        static float fpsHistory[90] = {};
        static int fpsHistoryOffset = 0;
        fpsHistory[fpsHistoryOffset] = fps;
        fpsHistoryOffset = (fpsHistoryOffset + 1) % IM_ARRAYSIZE(fpsHistory);
        ImGui::PlotLines("##fps", fpsHistory, IM_ARRAYSIZE(fpsHistory), fpsHistoryOffset,
                         nullptr, 0.0f, 144.0f, ImVec2(0, 40));

        if (m_scene)
            ImGui::Text("Objects: %zu", m_scene->getGameObjects().size());
    }

    void ImGuiLayer::drawSceneHierarchyPanel()
    {
        if (!ImGui::CollapsingHeader("Scene", ImGuiTreeNodeFlags_DefaultOpen))
            return;

        if (!m_scene)
        {
            ImGui::TextDisabled("No scene attached");
            return;
        }

        ImGui::BeginChild("SceneObjectList", ImVec2(0, 120), true);
        // getGameObjects() is the flat owning list (every object, not just roots -
        // see GameObject.hpp's ownership comment), so only top-level objects
        // (getParent() == nullptr) are entered here; everything else is reached
        // by walking getChildren() inside drawGameObjectNode() instead of
        // appearing a second time at this level.
        for (const auto &object : m_scene->getGameObjects())
        {
            if (object->getParent() == nullptr)
                drawGameObjectNode(object.get());
        }
        ImGui::EndChild();

        if (m_selectedObject && ImGui::Button("Deselect"))
            m_selectedObject = nullptr;
    }

    void ImGuiLayer::drawGameObjectNode(GameObject *object)
    {
        // Objects Scene::destroy() has marked for removal are still in the flat
        // list until end-of-frame cleanup (see GameObject::isPendingKill()) -
        // skip them so the tree doesn't flash dying objects for one frame.
        if (!object || object->isPendingKill())
            return;

        const std::vector<GameObject *> &children = object->getChildren();

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

        const bool hasChildren = !children.empty();
        if (!hasChildren)
            flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

        if (m_selectedObject == object)
            flags |= ImGuiTreeNodeFlags_Selected;

        ImGui::PushID(object);
        const bool opened = ImGui::TreeNodeEx(object->name.c_str(), flags);

        if (ImGui::IsItemClicked())
            m_selectedObject = object;

        if (opened && hasChildren)
        {
            for (GameObject *child : children)
                drawGameObjectNode(child);
            ImGui::TreePop();
        }

        ImGui::PopID();
    }

    void ImGuiLayer::drawInspectorPanel(GameObject *object)
    {
        if (!ImGui::CollapsingHeader("Inspector", ImGuiTreeNodeFlags_DefaultOpen))
            return;

        if (!object)
        {
            ImGui::TextDisabled("No object selected");
            return;
        }

        ImGui::Text("%s", object->name.c_str());
        ImGui::DragFloat3("Position", &object->transform.position.x, 0.05f);
        ImGui::DragFloat3("Rotation", &object->transform.rotation.x, 0.5f);
        ImGui::DragFloat3("Scale", &object->transform.scale.x, 0.05f);

        ImGui::Spacing();
        drawMaterialSection(object->material);
    }

    void ImGuiLayer::drawMaterialSection(Material *material)
    {
        if (!ImGui::TreeNodeEx("Material", ImGuiTreeNodeFlags_DefaultOpen))
            return;

        if (!material)
        {
            ImGui::TextDisabled("No material");
            ImGui::TreePop();
            return;
        }

        // getTextures() returns by value, so this iterates a snapshot -
        // read-only, matching Material's own const accessor.
        if (ImGui::TreeNode("Textures"))
        {
            std::vector<Material::TextureSlot> textures = material->getTextures();
            for (size_t i = 2; i < textures.size(); i++)
            {

                ImGui::Text("%s", textures.at(i).name.c_str());

                if (textures.at(i).texture)
                {
                    // Assumes Texture exposes its GL handle via getID() - rename
                    // to match the real accessor if it's called something else.
                    const ImTextureID textureId =
                        (ImTextureID)(intptr_t)textures.at(i).texture->getID();
                    ImGui::Image(textureId, ImVec2(64, 64));
                }
                else
                {
                    ImGui::TextDisabled("(null)");
                }

                ImGui::Spacing();
            }
            ImGui::TreePop();
        }

        // getProperties() is const, so each property is edited on a local copy
        // and written back through setProperty() rather than in place.
        if (ImGui::TreeNode("Properties"))
        {
            for (const auto &[name, property] : material->getProperties())
            {
                MaterialProperty value = property;
                bool changed = false;

                std::visit(
                    [&](auto &v)
                    {
                        using T = std::decay_t<decltype(v)>;

                        if constexpr (std::is_same_v<T, bool>)
                            changed = ImGui::Checkbox(name.c_str(), &v);
                        else if constexpr (std::is_same_v<T, int>)
                            changed = ImGui::DragInt(name.c_str(), &v);
                        else if constexpr (std::is_same_v<T, unsigned int>)
                            changed = ImGui::DragScalar(name.c_str(), ImGuiDataType_U32, &v, 1.0f);
                        else if constexpr (std::is_same_v<T, float>)
                            changed = ImGui::DragFloat(name.c_str(), &v, 0.05f);
                        else if constexpr (std::is_same_v<T, glm::vec2>)
                            changed = ImGui::DragFloat2(name.c_str(), &v.x, 0.05f);
                        else if constexpr (std::is_same_v<T, glm::vec3>)
                            changed = ImGui::DragFloat3(name.c_str(), &v.x, 0.05f);
                        else if constexpr (std::is_same_v<T, glm::vec4>)
                            changed = ImGui::DragFloat4(name.c_str(), &v.x, 0.05f);
                        else
                            // glm::mat3 / glm::mat4 - no compact widget, show read-only.
                            ImGui::TextDisabled("%s: matrix", name.c_str());
                    },
                    value);

                if (changed)
                    material->setProperty(name, value);
            }
            ImGui::TreePop();
        }

        ImGui::TreePop();
    }

    void ImGuiLayer::drawLightsPanel()
    {
        if (!ImGui::CollapsingHeader("Lights", ImGuiTreeNodeFlags_DefaultOpen))
            return;

        if (!m_scene)
        {
            ImGui::TextDisabled("No scene attached");
            return;
        }

        int lightIndex = 0;
        for (const auto &lightPtr : m_scene->getLights())
        {
            Light *light = lightPtr.get();
            ImGui::PushID(lightIndex);

            if (ImGui::TreeNode("##light", "%s Light", light->getTypeName().c_str()))
            {
                // Fields assumed common to the base Light class - rename/remove
                // if Light.hpp's actual members differ.
                ImGui::ColorEdit3("Color", &light->color.x);
                ImGui::DragFloat("Intensity", &light->intensity, 0.05f, 0.0f, 100.0f);

                if (auto *point = dynamic_cast<PointLight *>(light))
                {
                    ImGui::DragFloat3("Position", &point->position.x, 0.05f);
                    ImGui::DragFloat("Range", &point->range, 0.1f, 0.0f, 1000.0f);
                }
                else if (auto *spot = dynamic_cast<SpotLight *>(light))
                {
                    ImGui::DragFloat3("Position", &spot->position.x, 0.05f);
                    ImGui::DragFloat3("Direction", &spot->direction.x, 0.01f);
                    ImGui::DragFloat("Range", &spot->range, 0.1f, 0.0f, 1000.0f);
                    ImGui::DragFloat("Inner Cone", &spot->innerCone, 0.1f, 0.0f, spot->outerCone);
                    ImGui::DragFloat("Outer Cone", &spot->outerCone, 0.1f, spot->innerCone, 90.0f);
                }
                else if (auto *directional = dynamic_cast<DirectionalLight *>(light))
                {
                    ImGui::DragFloat3("Direction", &directional->direction.x, 0.01f);
                }

                ImGui::TreePop();
            }

            ImGui::PopID();
            ++lightIndex;
        }

        if (lightIndex == 0)
            ImGui::TextDisabled("No lights in scene");
    }
}