#ifndef NEON_TEXTURE2D
#define NEON_TEXTURE2D
#include <filesystem>

namespace Neon
{
    class Texture
    {
    public:
        Texture(const std::filesystem::path &path);

        ~Texture();

        void bind(unsigned int slot = 0) const;
        static void unbind();

        int width() const;
        int height() const;

    private:
        unsigned int m_id;

        int m_width;
        int m_height;
        int m_channels;
    };
}

#endif