#include <iostream>

#include "Texture.hpp"
#include "glad/glad.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Neon
{
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
            std::cout << "[NEON][ERROR] Failed to load texture: "
                      << path << '\n';
            return;
        }

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
            GL_NEAREST);

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
        glDeleteTextures(1, &m_id);
    }

    void Texture::bind(unsigned int slot) const
    {
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