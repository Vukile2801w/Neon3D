#ifndef NEON_SHADER
#define NEON_SHADER

#include <filesystem>

#include "Rendering/ShaderStage.hpp"
#include "Assets/AssetLoader.hpp"
#include "Ref.hpp"

namespace Neon
{

    enum class ShaderDataType
    {
        Float,
        Int,
        Double
    };

    // The linked, bindable GL program - built from a vertex ShaderStage and a
    // fragment ShaderStage (see ShaderStage.hpp for why a program can't be loaded
    // from a single path). Not itself an AssetLoader<T> target for that reason;
    // build one directly once both stages are loaded/available:
    //
    //   Ref<ShaderStage> vertex   = assetManager.load<ShaderStage>("foo.vert");
    //   Ref<ShaderStage> fragment = assetManager.load<ShaderStage>("foo.frag");
    //   Ref<Shader> shader        = std::make_shared<Shader>(vertex, fragment);
    class Shader
    {
    public:
        // Pass a ShaderStage constructed with an empty path (see ShaderStage's own
        // constructor) for either argument to fall back to the built-in default
        // vertex/fragment source, matching the previous "pass empty path" behavior.
        Shader(
            Ref<ShaderStage> vertex,
            Ref<ShaderStage> fragment);

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

        unsigned int linkProgram(unsigned int vertexShader, unsigned int fragmentShader);

        unsigned int m_program{};
    };

    template <>
    struct AssetLoader<Shader>
    {
        static Ref<Shader> load(
            const Ref<ShaderStage> &vertex,
            const Ref<ShaderStage> &fragment)
        {
            return std::make_shared<Shader>(
                vertex,
                fragment);
        }
    };
}

#endif