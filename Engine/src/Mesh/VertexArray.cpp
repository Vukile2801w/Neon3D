#include "Mesh/VertexArray.hpp"

#include <initializer_list>
#include <cstdlib>

#include "Rendering/Shader.hpp"
#include "Assert.hpp"
#include "Logging.hpp"

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

        default:
            NEON_ASSERT(false, "Unknown ShaderDataType");
            return 0;
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
        if (elements.size() == 0)
        {
            Logging::Warning(
                "VertexArray::addBuffer() received an empty vertex layout");
        }

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

            NEON_ASSERT(element.size >= 1 && element.size <= 4,
                        "BufferElement size must be between 1 and 4 components");

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
