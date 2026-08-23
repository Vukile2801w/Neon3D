#include "VertexBuffer.hpp"

#include "Logging.hpp"

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

        Logging::Info("VBO created");
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
