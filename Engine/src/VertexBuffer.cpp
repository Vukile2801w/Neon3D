#include <iostream>
#include "VertexBuffer.hpp"
#include "glad/glad.h"
#include "glfw/glfw3.h"

namespace Neon
{
    VertexBuffer::VertexBuffer(
        const void *data,
        size_t size)
    {
        glGenBuffers(1, &m_id);

        glBindBuffer(
            GL_ARRAY_BUFFER,
            m_id);

        glBufferData(
            GL_ARRAY_BUFFER,
            size,
            data,
            GL_STATIC_DRAW);

        std::cout << "\033[32m[NEON][INFO] - VBO created\033[0m\n";
        }

    VertexBuffer::~VertexBuffer()
    {
        glDeleteBuffers(1, &m_id);
    }

    void VertexBuffer::bind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_id);
    }
} // namespace Neon
