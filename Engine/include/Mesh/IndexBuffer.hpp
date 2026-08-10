#ifndef NEON_INDEXBUFFER
#define NEON_INDEXBUFFER

#include <cstddef>

namespace Neon
{
    class IndexBuffer
    {
    public:
        IndexBuffer(const void *data, size_t size);
        ~IndexBuffer();

        void bind() const;
        static void unbind();

    private:
        unsigned int m_id{};
    };
}

#endif