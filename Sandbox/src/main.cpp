#include <iterator>
#include <chrono>

#include "Window.hpp"
#include "Shader.hpp"
#include "VertexArray.hpp"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"

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
        // positions           // colors
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f    // top
    };
    unsigned int indices[] = {
        // note that we start from 0!
        0,
        1,
        2, // first triangle
    };

    auto VBO = std::make_unique<Neon::VertexBuffer>(
        vertices,
        sizeof(vertices));
    std::unique_ptr<Neon::VertexArray> VAO = std::make_unique<Neon::VertexArray>();
    VAO->addBuffer(*VBO,
                   {{3, Neon::ShaderDataType::Float},
                    {3, Neon::ShaderDataType::Float}});

    auto EBO = std::make_unique<Neon::IndexBuffer>(indices, sizeof(indices));

    auto shader = std::make_unique<Neon::Shader>(
        "C:/Users/wukbg/programing/C++/Neon3D/Sandbox/shaders/shader.vert",
        "C:/Users/wukbg/programing/C++/Neon3D/Sandbox/shaders/shader.frag");

    while (!window->shoudWindowsClose())
    {
        shader->bind();
        shader->setFloat("time", getTime());
        VAO->bind();
        window->render();
    }

    return 0;
}