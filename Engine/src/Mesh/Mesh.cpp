#include "glad/glad.h"
#include "glfw/glfw3.h"

#include "Mesh.hpp"

namespace Neon
{
    Mesh::Mesh(
        const void *vertices,
        size_t vertexSize,
        const unsigned int *indices,
        size_t indexSize,
        std::initializer_list<BufferElement> layout)
        : m_vertexBuffer(vertices, vertexSize),
          m_vertexArray(),
          m_indexBuffer(indices, indexSize),
          m_indexCount(indexSize / sizeof(unsigned int))
    {
        m_vertexArray.addBuffer(m_vertexBuffer, layout);
    }

    void Mesh::draw() const
    {
        m_vertexArray.bind();

        glDrawElements(
            GL_TRIANGLES,
            static_cast<GLsizei>(m_indexCount),
            GL_UNSIGNED_INT,
            nullptr);
    }
}