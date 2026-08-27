#include "Rendering/Texture.hpp"

#include "Logging.hpp"
#include "Assert.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "glad/glad.h"
#include "stb_image.h"

namespace Neon
{
    GLenum toGLTextureFilter(TextureFilter filter)
    {
        switch (filter)
        {
        case TextureFilter::Nearest:
            return GL_NEAREST;

        case TextureFilter::Linear:
            return GL_LINEAR;

        case TextureFilter::NearestMipmapNearest:
            return GL_NEAREST_MIPMAP_NEAREST;

        case TextureFilter::LinearMipmapNearest:
            return GL_LINEAR_MIPMAP_NEAREST;

        case TextureFilter::NearestMipmapLinear:
            return GL_NEAREST_MIPMAP_LINEAR;

        case TextureFilter::LinearMipmapLinear:
            return GL_LINEAR_MIPMAP_LINEAR;
        }

        return GL_LINEAR;
    }

    GLenum toGLTextureWrap(TextureWrap wrap)
    {
        switch (wrap)
        {
        case TextureWrap::Repeat:
            return GL_REPEAT;

        case TextureWrap::MirroredRepeat:
            return GL_MIRRORED_REPEAT;

        case TextureWrap::ClampToEdge:
            return GL_CLAMP_TO_EDGE;

        case TextureWrap::ClampToBorder:
            return GL_CLAMP_TO_BORDER;
        }

        return GL_REPEAT;
    }

    Texture::Texture(const std::filesystem::path &path)
    {
        stbi_set_flip_vertically_on_load(true);

        unsigned char *data = stbi_load(
            path.string().c_str(),
            &m_width,
            &m_height,
            &m_channels,
            0);

        if (!data)
        {
            Logging::Error("Failed to load texture: " + path.string());
            return;
        }
        Logging::Info("Texture loaded: " + path.string());

        GLenum format = GL_RGB;

        if (m_channels == 4)
            format = GL_RGBA;
        else if (m_channels == 1)
            format = GL_RED;

        glGenTextures(1, &m_id);
        glBindTexture(GL_TEXTURE_2D, m_id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_MIN_FILTER,
            GL_LINEAR_MIPMAP_LINEAR);

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_MAG_FILTER,
            GL_LINEAR);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            format,
            m_width,
            m_height,
            0,
            format,
            GL_UNSIGNED_BYTE,
            data);

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
    }

    Texture::~Texture()
    {
        Logging::Info("Texture deleted");
        glDeleteTextures(1, &m_id);
    }

    void Texture::setFilter(TextureFilter min, TextureFilter mag)
    {
        NEON_ASSERT(m_id != 0, "Attempting to configure filter on an invalid texture");
        glBindTexture(GL_TEXTURE_2D, m_id);

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_MIN_FILTER,
            toGLTextureFilter(min));

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_MAG_FILTER,
            toGLTextureFilter(mag));
    }

    void Texture::setWrap(TextureWrap s, TextureWrap t)
    {
        NEON_ASSERT(m_id != 0, "Attempting to configure wrap mode on an invalid texture");
        glBindTexture(GL_TEXTURE_2D, m_id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, toGLTextureWrap(s));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, toGLTextureWrap(t));
    }

    void Texture::bind(unsigned int slot) const
    {
        NEON_ASSERT(m_id != 0, "Attempting to bind an invalid texture");

        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_id);
    }

    void Texture::unbind()
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    int Texture::width() const
    {
        return m_width;
    }

    int Texture::height() const
    {
        return m_height;
    }
}