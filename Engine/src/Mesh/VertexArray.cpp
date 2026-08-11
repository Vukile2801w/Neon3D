#include <initializer_list>
#include <cstdlib>

#include "Logging.hpp"
#include "VertexArray.hpp"
#include "Shader.hpp"
#include "glad/glad.h"
#include "glfw/glfw3.h"

namespace Neon
{

    size_t getTypeSize(ShaderDataType type)
    {
        switch (type)
        {
        case ShaderDataType::Float:
            return sizeof(float);

        case ShaderDataType::Int:
            return sizeof(int);

        case ShaderDataType::Double:
            return sizeof(double);
        }

        return 0;
    }

    GLenum getGlType(ShaderDataType type)
    {
        switch (type)
        {
        case ShaderDataType::Float:
            return GL_FLOAT;
            break;

        case ShaderDataType::Int:
            return GL_INT;
            break;

        case ShaderDataType::Double:
            return GL_DOUBLE;
            break;

        default:
            return -1;
            break;
        }
    }

    VertexArray::VertexArray()
    {
        glGenVertexArrays(1, &m_id);
        glBindVertexArray(m_id);
        Logging::Info("VAO created");
    }

    VertexArray::~VertexArray()
    {
        glDeleteVertexArrays(1, &m_id);
    }

    void VertexArray::bind() const
    {
        glBindVertexArray(m_id);
    }

    void VertexArray::unbind() const
    {
        glBindVertexArray(0);
    }

    void VertexArray::addBuffer(
        const VertexBuffer &buffer,
        std::initializer_list<BufferElement> elements)
    {
        bind();
        buffer.bind();

        size_t stride = 0;
        for (const auto &element : elements)
        {
            stride += element.size * getTypeSize(element.type);
        }

        size_t offset = 0;
        uint32_t index = 0;

        for (const auto &element : elements)
        {
            GLenum type = getGlType(element.type);

            if (type == static_cast<GLenum>(-1))
            {
                Logging::Error("Unknown type passed as attribute");
                return;
            }

            glVertexAttribPointer(
                index,
                element.size,
                type,
                GL_FALSE,
                stride,
                reinterpret_cast<void *>(offset));

            glEnableVertexAttribArray(index);

            offset += element.size * getTypeSize(element.type);
            index++;
        }

        Logging::Info("VBO added to VAO");
    }
} // namespace Neon
