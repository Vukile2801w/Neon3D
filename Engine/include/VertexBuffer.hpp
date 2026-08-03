#ifndef NEON_VERTEX_BUFFER
#define NEON_VERTEX_BUFFER
#include <cstddef>

namespace Neon
{
    // Vertex Buffer Object
    class VertexBuffer
    {
    public:
        VertexBuffer(const void *data, size_t size);
        ~VertexBuffer();

        void bind() const;
        void unbind() const;

    private:
        unsigned int m_id;
    };
}

#endif