#include "Rendering/Shader.hpp"

#include <string>

#include "Assert.hpp"
#include "Logging.hpp"

#include "glad/glad.h"
#include "glfw/glfw3.h"

namespace Neon
{
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
            Logging::Error("Program failed to link\n" + std::string(infoLog));
            return 0;
        }

        Logging::Info("Shaders linked");
        return shaderProgram;
    }

    Shader::Shader(
        Ref<ShaderStage> vertex,
        Ref<ShaderStage> fragment)
    {
        NEON_ASSERT(vertex != nullptr, "Shader constructed with a null vertex ShaderStage");
        NEON_ASSERT(fragment != nullptr, "Shader constructed with a null fragment ShaderStage");

        if (vertex->getId() == 0 || fragment->getId() == 0)
        {
            // One of the stages failed to compile - already logged by ShaderStage
            // itself (a runtime failure, not a programmer error), so just bail out
            // and leave m_program == 0 (an invalid/unlinked shader).
            return;
        }

        m_program = linkProgram(vertex->getId(), fragment->getId());

        m_stagesPath[vertex->getType()] = vertex->getPath();
        m_stagesPath[fragment->getType()] = fragment->getPath();
    }

    void Shader::bind() const
    {
        NEON_ASSERT(m_program != 0, "Attempting to bind an invalid/unlinked shader");
        glUseProgram(m_program);
    }
    void Shader::unbind()
    {
        glUseProgram(0);
    }

    std::string Shader::getStageSourcePath(ShaderStage::Type type)
    {
        return m_stagesPath.at(type);
    }

    int Shader::getUniformLocation(const std::string &name) const
    {
        NEON_ASSERT(m_program != 0, "Attempting to query a uniform on an invalid shader");

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