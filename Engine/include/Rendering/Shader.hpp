#ifndef NEON_SHADER
#define NEON_SHADER
#include <filesystem>

namespace Neon
{

    enum class ShaderDataType
    {
        Float,
        Int,
        Double
    };

    class Shader
    {
    public:
        enum class ShaderType
        {
            FragmentShader,
            VertexShader
        };

        Shader(
            const std::filesystem::path &vertex,
            const std::filesystem::path &fragment); // Pass empty path string for default shader

        void bind() const;
        static void unbind();

        // Adding uniforms uniforms

        // Uniforms
        void setBool(const std::string &name, bool value) const;
        void setInt(const std::string &name, int value) const;
        void setUInt(const std::string &name, unsigned int value) const;
        void setFloat(const std::string &name, float value) const;

        void setVec2(const std::string &name,
                     float x, float y) const;

        void setVec3(const std::string &name,
                     float x, float y, float z) const;

        void setVec4(const std::string &name,
                     float x, float y, float z, float w) const;

        void setMat3(const std::string &name,
                     const float *matrix) const;

        void setMat4(const std::string &name,
                     const float *matrix) const;

    private:
        int getUniformLocation(const std::string &name) const;

        const char *readFile(const std::filesystem::path &path);
        unsigned int compileShader(const std::filesystem::path &path, Shader::ShaderType type);
        unsigned int linkProgram(unsigned int vertexShader, unsigned int fragmentShader);

        unsigned int m_program{};
    };
}

#endif