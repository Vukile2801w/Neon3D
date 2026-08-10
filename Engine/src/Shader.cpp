#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#include "Shader.hpp"
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

    const char *Shader::readFile(const std::filesystem::path &path)
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

    unsigned int Shader::compileShader(const std::filesystem::path &path, Shader::ShaderType type)
    {
        int success;
        char infoLog[512];

        const char *shaderSource;

        if (path.empty())
        {
            shaderSource =
                type == Shader::ShaderType::FragmentShader
                    ? DefaultFragmentShaderSource
                    : DefaultVertexShaderSource;
        }
        else
        {
            shaderSource = readFile(path);
            if (shaderSource == nullptr)
            {
                std::cout << "\033[31m[ERROR][NEON] - Failed to load "
                          << (type == Shader::ShaderType::FragmentShader ? "fragment" : "vertex")
                          << " shader, file not found\033[0m" << std::endl;
                return 0;
            }
        }

        unsigned int shader;
        shader = glCreateShader(type == Shader::ShaderType::FragmentShader ? GL_FRAGMENT_SHADER : GL_VERTEX_SHADER);

        glShaderSource(shader, 1, &shaderSource, NULL);
        glCompileShader(shader);

        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            std::cout << "\033[31m[NEON][ERROR] -  " << (type == Shader::ShaderType::FragmentShader ? "Fragment" : "Vertex")
                      << " shader failed to compile\033[0m\n"
                      << infoLog << std::endl;
            return 0;
        }

        std::cout << "\033[32m[NEON][INFO] - Shader " << path << " is compiled \033[0m\n";

        return shader;
    }

    unsigned int Shader::linkProgram(unsigned int vertexShader, unsigned int fragmentShader)
    {
        int success;
        char infoLog[512];

        unsigned int shaderProgram;
        shaderProgram = glCreateProgram();

        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            std::cout << "\033[31m[NEON][ERROR] - Program failed to link\033[0m\n"
                      << infoLog << std::endl;
            return 0;
        }

        std::cout << "\033[32m[NEON][INFO] - Shaders linked\033[0m\n";
        return shaderProgram;
    }

    Shader::Shader(
        const std::filesystem::path &vertex,
        const std::filesystem::path &fragment)
    {
        unsigned int vertexShader =
            compileShader(vertex, ShaderType::VertexShader);

        if (vertexShader == 0)
        {
            return;
        }
        unsigned int fragmentShader =
            compileShader(fragment, ShaderType::FragmentShader);

        if (fragmentShader == 0)
        {
            glDeleteShader(vertexShader);
            return;
        }

        m_program = linkProgram(vertexShader, fragmentShader);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }
    void Shader::bind() const
    {
        glUseProgram(m_program);
    }
    void Shader::unbind()
    {
        glUseProgram(0);
    }

    int Shader::getUniformLocation(const std::string &name) const
    {
        return glGetUniformLocation(m_program, name.c_str());
    }

    void Shader::setBool(const std::string &name, bool value) const
    {
        glUniform1i(getUniformLocation(name), value);
    }

    void Shader::setInt(const std::string &name, int value) const
    {
        glUniform1i(getUniformLocation(name), value);
    }

    void Shader::setUInt(const std::string &name, unsigned int value) const
    {
        glUniform1ui(getUniformLocation(name), value);
    }

    void Shader::setFloat(const std::string &name, float value) const
    {
        glUniform1f(getUniformLocation(name), value);
    }

    void Shader::setVec2(
        const std::string &name,
        float x, float y) const
    {
        glUniform2f(
            getUniformLocation(name),
            x, y);
    }

    void Shader::setVec3(
        const std::string &name,
        float x, float y, float z) const
    {
        glUniform3f(
            getUniformLocation(name),
            x, y, z);
    }

    void Shader::setVec4(
        const std::string &name,
        float x, float y, float z, float w) const
    {
        glUniform4f(
            getUniformLocation(name),
            x, y, z, w);
    }

    void Shader::setMat3(
        const std::string &name,
        const float *matrix) const
    {
        glUniformMatrix3fv(
            getUniformLocation(name),
            1,
            GL_FALSE,
            matrix);
    }

    void Shader::setMat4(
        const std::string &name,
        const float *matrix) const
    {
        glUniformMatrix4fv(
            getUniformLocation(name),
            1,
            GL_FALSE,
            matrix);
    }

} // namespace Neon
