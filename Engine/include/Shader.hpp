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

    private:
        const char *readFile(const std::filesystem::path &path);
        unsigned int compileShader(const std::filesystem::path &path, Shader::ShaderType type);
        unsigned int linkProgram(unsigned int vertexShader, unsigned int fragmentShader);

        unsigned int m_program{};
    };
}

#endif