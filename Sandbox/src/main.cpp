#include <iterator>
#include <chrono>

#include "Material.hpp"
#include "Window.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"
#include "Texture.hpp"

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
        // Tacke pravugaonika + UV kordinate
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f};

    unsigned int indices[] = {
        // Redosled crtanja trouglova
        0, 1, 2,
        0, 2, 3};

    // Kreiranje mesha sa prosledjenim podacima
    auto mesh = std::make_unique<Neon::Mesh>(
        vertices, sizeof(vertices),
        indices, sizeof(indices),
        std::initializer_list<Neon::BufferElement>{
            {3, Neon::ShaderDataType::Float},
            {2, Neon::ShaderDataType::Float}});

    // Ucitavanje shadera
    Neon::Shader shader(
        "C:/Users/wukbg/programing/C++/Neon3D/Sandbox/shaders/shader.vert",
        "C:/Users/wukbg/programing/C++/Neon3D/Sandbox/shaders/shader.frag");

    // Kreiranje materijala
    Neon::Material mat(shader);

    // Kreiranje texture i podesavanje filtera
    Neon::Texture texture("c:/Users/wukbg/programing/C++/Neon3D/Sandbox/assets/tile.png");
    texture.setFilter(Neon::TextureFilter::NearestMipmapNearest, Neon::TextureFilter::Nearest);

    // Dodavanje texture u material
    mat.setTexture("texture1", texture, 0);

    while (!window->shoudWindowsClose())
    {
        mat.bind();   // Aktivitranje materijala
        mesh->draw(); // Iscrtavanje mesha

        window->render(); // Osvezavanje ekrana
    }

    return 0;
}