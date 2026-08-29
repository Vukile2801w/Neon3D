#include "Rendering/ShaderStage.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#include "Logging.hpp"

#include "glad/glad.h"
#include "glfw/glfw3.h"

namespace Neon
{
    const char *DefaultVertexShaderSource = "#version 420 core\n"
                                            "layout (location = 0) in vec3 aPos;\n"
                                            "void main()\n"
                                            "{\n"
                                            "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                            "}\0";

    const char *DefaultFragmentShaderSource = "#version 420 core\n"
                                              "out vec4 FragColor;\n"
                                              "void main()\n"
                                              "{\n"
                                              "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                              "}\0";

    const char *ShaderStage::readFile(const std::filesystem::path &path)
    {
        static std::string buffer;

        std::ifstream file(path);

        if (!file.is_open())
            return nullptr;

        std::stringstream ss;
        ss << file.rdbuf();

        buffer = ss.str();

        return buffer.c_str();
    }

    ShaderStage::ShaderStage(const std::filesystem::path &path, Type type)
        : m_type(type)
    {
        int success;
        char infoLog[512];

        const char *shaderSource;

        if (path.empty())
        {
            shaderSource =
                type == Type::Fragment
                    ? DefaultFragmentShaderSource
                    : DefaultVertexShaderSource;
        }
        else
        {
            shaderSource = readFile(path);

            if (shaderSource == nullptr)
            {
                const char *shaderType = type == Type::Fragment ? "fragment" : "vertex";

                std::string msg = "Failed to load ";
                msg += shaderType;
                msg += " shader, file not found";

                Logging::Error(msg);
                return;
            }
        }

        m_id = glCreateShader(type == Type::Fragment ? GL_FRAGMENT_SHADER : GL_VERTEX_SHADER);

        glShaderSource(m_id, 1, &shaderSource, NULL);
        glCompileShader(m_id);

        glGetShaderiv(m_id, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(m_id, 512, NULL, infoLog);

            const char *shaderType = type == Type::Fragment ? "fragment" : "vertex";

            std::string msg = "Failed to compile ";
            msg += shaderType;
            msg += " shader:\n";
            msg += infoLog;

            Logging::Error(msg);

            glDeleteShader(m_id);
            m_id = 0;

            return;
        }

        Logging::Info("Shader " + path.string() + " is compiled");
    }

    ShaderStage::~ShaderStage()
    {
        if (m_id != 0)
        {
            glDeleteShader(m_id);
        }
    }
}