#include <iterator>
#include <chrono>

#include "Logging.hpp"
#include "Material.hpp"
#include "Window.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"
#include "Texture.hpp"
#include "Transform.hpp"
#include "Camera.hpp"

// Enums
using Neon::Logging;
using Neon::ShaderDataType;
using Neon::TextureFilter;

float getTime()
{
    using namespace std::chrono;

    static const auto start = high_resolution_clock::now();

    auto now = high_resolution_clock::now();

    return duration<float>(now - start).count();
}

int main()
{
    std::unique_ptr<Neon::Window> window =
        std::make_unique<Neon::Window>(); // Kreiranje prozora

    float vertices[] = {
        // position
        //  x      y      z

        // Front
        -0.5f, -0.5f, 0.5f, // 0
        0.5f, -0.5f, 0.5f,  // 1
        0.5f, 0.5f, 0.5f,   // 2
        -0.5f, 0.5f, 0.5f,  // 3

        // Back
        -0.5f, -0.5f, -0.5f, // 4
        0.5f, -0.5f, -0.5f,  // 5
        0.5f, 0.5f, -0.5f,   // 6
        -0.5f, 0.5f, -0.5f   // 7
    };

    unsigned int indices[] = {
        // Front
        0, 1, 2,
        2, 3, 0,
        // Right
        1, 5, 6,
        6, 2, 1,
        // Back
        5, 4, 7,
        7, 6, 5,
        // Left
        4, 0, 3,
        3, 7, 4,
        // Top
        3, 2, 6,
        6, 7, 3,
        // Bottom
        4, 5, 1,
        1, 0, 4};

    // Kreiranje mesha sa prosledjenim podacima
    auto mesh = std::make_unique<Neon::Mesh>(
        vertices, sizeof(vertices),
        indices, sizeof(indices),
        std::initializer_list<Neon::BufferElement>{
            {3, ShaderDataType::Float}});

    // Ucitavanje shadera
    Neon::Shader shader(
        "C:/Users/wukbg/programing/C++/Neon3D/Sandbox/shaders/shader.vert",
        "C:/Users/wukbg/programing/C++/Neon3D/Sandbox/shaders/shader.frag");

    // Kreiranje materijala
    Neon::Material mat(shader);

    // Kreiranje texture i podesavanje filtera
    Neon::Texture texture("c:/Users/wukbg/programing/C++/Neon3D/Sandbox/assets/tile.png");
    texture.setFilter(TextureFilter::NearestMipmapNearest, TextureFilter::Nearest);

    Neon::Camera camera;
    camera.position.y = 1.3f;
    camera.rotation.x = glm::radians(-20.0f);

    Neon::Transform cubeTransform;
    cubeTransform.position = {0.0f, 0.0f, 0.0f};

    float cTime = getTime();
    float lTime;
    while (!window->shoudWindowsClose())
    {
        lTime = cTime;
        cTime = getTime();

        cubeTransform.rotation += glm::vec3(
            0.0f,
            cTime - lTime,
            0.0f);

        float aspect =
            static_cast<float>(window->getWidth()) /
            static_cast<float>(window->getHeight());

        mat.bind(); // Aktivitranje materijala
        mat.set("u_Model", cubeTransform.getMatrix());
        mat.set("u_View", camera.getViewMatrix());
        mat.set("u_Projection", camera.getProjectionMatrix(aspect));
        mat.set("u_time", getTime());
        mat.set("u_color", glm::vec3(0.1f, 0.8f, 1.0f));

        mesh->draw(); // Iscrtavanje mesha

        window->render(); // Osvezavanje ekrana
    }

    return 0;
}