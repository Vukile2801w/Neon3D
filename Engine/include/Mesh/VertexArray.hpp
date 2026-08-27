#ifndef NEON_VERTEX_ARRAY
#define NEON_VERTEX_ARRAY

#include "Rendering/Shader.hpp"
#include "VertexBuffer.hpp"

namespace Neon
{
    struct BufferElement
    {
        int size;
        ShaderDataType type;
    };

    class VertexArray
    {
    public:
        VertexArray();
        ~VertexArray();

        void bind() const;
        void unbind() const;

        void addBuffer(
            const VertexBuffer &buffer,
            std::initializer_list<BufferElement> elements);

    private:
        unsigned int m_id;
    };
}

#endif