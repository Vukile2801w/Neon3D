#include "Mesh/Mesh.hpp"

#include "Assert.hpp"

#include "glad/glad.h"
#include "glfw/glfw3.h"

namespace Neon
{
    static size_t validateVertexSize(size_t vertexSize, const void *vertices)
    {
        NEON_ASSERT(
            vertices != nullptr || vertexSize == 0,
            "Mesh vertex data is null while vertex size is non-zero");

        return vertexSize;
    }

    static size_t validateIndexBuffer(size_t indexSize, const unsigned int *indices)
    {
        NEON_ASSERT(
            indexSize % sizeof(unsigned int) == 0,
            "Mesh index buffer size must be a multiple of sizeof(unsigned int)");

        NEON_ASSERT(
            indices != nullptr || indexSize == 0,
            "Mesh index data is null while index size is non-zero");

        return indexSize;
    }

    Mesh::Mesh(
        const void *vertices,
        size_t vertexSize,
        const unsigned int *indices,
        size_t indexSize,
        const std::vector<BufferElement> &layout)
        : m_vertexBuffer(
              vertices,
              validateVertexSize(vertexSize, vertices)),
          m_vertexArray(),
          m_indexBuffer(
              indices,
              validateIndexBuffer(indexSize, indices)),
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