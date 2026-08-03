#include <iterator>

#include "Window.hpp"
#include "Shader.hpp"
#include "VertexArray.hpp"
#include "VertexBuffer.hpp"

int main()
{
    std::unique_ptr<Neon::Window> window =
        std::make_unique<Neon::Window>();

    float vertices[] = {
        // prvi trougao
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,

        // drugi trougao
        -1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f};

    auto VBO = std::make_unique<Neon::VertexBuffer>(
        vertices,
        sizeof(vertices));
    std::unique_ptr<Neon::VertexArray> VAO = std::make_unique<Neon::VertexArray>();
    VAO->addBuffer(*VBO,
                   {{3, Neon::ShaderDataType::Float}});
    auto shader = std::make_unique<Neon::Shader>("", "C:/Users/wukbg/programing/C++/Neon3D/Sandbox/shaders/shader.frag");

    while (!window->shoudWindowsClose())
    {
        shader->bind();
        VAO->bind();
        window->render();
    }

    return 0;
}