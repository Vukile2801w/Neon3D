#ifndef NEON_TEXTURE2D
#define NEON_TEXTURE2D
#include <filesystem>

namespace Neon
{
    enum class TextureFilter
    {
        Linear,
        Nearest,
        LinearMipmapLinear,
        LinearMipmapNearest,
        NearestMipmapLinear,
        NearestMipmapNearest
    };
    enum class TextureWrap
    {
        Repeat,
        MirroredRepeat,
        ClampToEdge,
        ClampToBorder
    };

    class Texture
    {
    public:
        Texture(const std::filesystem::path &path);

        ~Texture();

        void setFilter(TextureFilter min, TextureFilter mag);
        void setWrap(TextureWrap s, TextureWrap t);

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