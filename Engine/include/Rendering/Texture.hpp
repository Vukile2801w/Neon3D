#ifndef NEON_TEXTURE2D
#define NEON_TEXTURE2D

#include <filesystem>

#include "Ref.hpp"
#include "Assets/AssetLoader.hpp"

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

        const std::string getPath();

    private:
        unsigned int m_id{};

        int m_width{};
        int m_height{};
        int m_channels{};

        std::string m_path;
    };

    struct TextureLoadOptions
    {
        TextureFilter minFilter = TextureFilter::Linear;
        TextureFilter magFilter = TextureFilter::Linear;

        TextureWrap wrapS = TextureWrap::Repeat;
        TextureWrap wrapT = TextureWrap::Repeat;
    };

    template <>
    struct AssetLoader<Texture>
    {

        static Ref<Texture> load(
            const std::filesystem::path &path,
            const TextureLoadOptions &options = {})
        {
            Ref<Texture> tex = std::make_shared<Texture>(path);

            tex->setFilter(
                options.minFilter,
                options.magFilter);

            tex->setWrap(
                options.wrapS,
                options.wrapT);

            return tex;
        }
    };

}

#endif