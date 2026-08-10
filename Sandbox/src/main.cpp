#include <iterator>
#include <chrono>

#include "Window.hpp"
#include "Shader.hpp"
#include "VertexArray.hpp"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
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
        std::make_unique<Neon::Window>();

    float vertices[] = {
        // positions          // colors           // texture coords
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // top left
    };
    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3};

    auto VBO = std::make_unique<Neon::VertexBuffer>(
        vertices,
        sizeof(vertices));
    std::unique_ptr<Neon::VertexArray> VAO = std::make_unique<Neon::VertexArray>();
    VAO->addBuffer(*VBO,
                   {
                       {3, Neon::ShaderDataType::Float}, // position
                       {3, Neon::ShaderDataType::Float}, // color
                       {2, Neon::ShaderDataType::Float}  // texture coords
                   });

    auto EBO = std::make_unique<Neon::IndexBuffer>(indices, sizeof(indices));

    auto shader = std::make_unique<Neon::Shader>(
        "C:/Users/wukbg/programing/C++/Neon3D/Sandbox/shaders/shader.vert",
        "C:/Users/wukbg/programing/C++/Neon3D/Sandbox/shaders/shader.frag");

    auto texture = std::make_unique<Neon::Texture>("c:/Users/wukbg/programing/C++/Neon3D/Sandbox/assets/tile.png");
    texture->bind(0);
    shader->setInt("texture1", 0);

    while (!window->shoudWindowsClose())
    {
        shader->bind();

        VAO->bind();

        window->render();
    }

    return 0;
}