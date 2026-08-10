#ifndef NEON_MESH
#define NEON_MESH
#include <memory>

#include "Mesh/IndexBuffer.hpp"
#include "Mesh/VertexArray.hpp"
#include "Mesh/VertexBuffer.hpp"

namespace Neon
{
    class Mesh
    {
    public:
        Mesh(
            const void *vertices,
            size_t vertexSize,
            const unsigned int *indices,
            size_t indexSize,
            std::initializer_list<BufferElement> layout);

        ~Mesh() = default;

        void draw() const;

    private:
        VertexBuffer m_vertexBuffer;
        VertexArray m_vertexArray;
        IndexBuffer m_indexBuffer;

        size_t m_indexCount;
    };
}

#endif