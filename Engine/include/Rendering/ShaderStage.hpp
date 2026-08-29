#ifndef NEON_SHADER_STAGE
#define NEON_SHADER_STAGE

#include <filesystem>

#include "Assets/AssetLoader.hpp"
#include "Logging.hpp"
#include "Ref.hpp"

namespace Neon
{
    // A single compiled-but-unlinked GL shader object - either a vertex stage or a
    // fragment stage. On its own it can't be bound/drawn with; it only becomes usable
    // once a Shader links a vertex ShaderStage together with a fragment ShaderStage.
    //
    // Splitting this out from Shader means two Shader programs that happen to share
    // the same vertex (or fragment) stage - a common case: one vertex shader, many
    // fragment variants - only need to compile that shared stage once, and it can be
    // cached/shared via AssetManager like any other single-path asset.
    class ShaderStage
    {
    public:
        enum class Type
        {
            Vertex,
            Fragment
        };

        // 'path' may be empty, which selects a minimal built-in default source for
        // 'type' (matching Shader's previous "pass empty path for default shader"
        // behavior). Logs and leaves the stage invalid (getId() == 0) on a read or
        // compile failure - a missing/bad shader file is a runtime failure, not a
        // programmer error, so this follows Logging::Error rather than NEON_ASSERT
        // (see CONTRIBUTING §5).
        ShaderStage(const std::filesystem::path &path, Type type);

        ~ShaderStage();

        Type getType() const { return m_type; }

        // Non-owning GL handle accessor - Shader::linkProgram() attaches this id to a
        // program but does not take ownership of it; the ShaderStage that created it
        // still deletes it in its own destructor.
        unsigned int getId() const { return m_id; }

    private:
        const char *readFile(const std::filesystem::path &path);

        unsigned int m_id{};
        Type m_type;
    };

    template <>
    struct AssetLoader<ShaderStage>
    {
        // Infers Vertex vs. Fragment from the file extension (.vert / .frag) so a
        // ShaderStage can be loaded from a single path, the same shape as every other
        // AssetLoader<T>::load(path) specialization.
        static Ref<ShaderStage> load(
            const std::filesystem::path &path)
        {
            const std::string extension = path.extension().string();

            ShaderStage::Type type;

            if (extension == ".vert")
            {
                type = ShaderStage::Type::Vertex;
            }
            else if (extension == ".frag")
            {
                type = ShaderStage::Type::Fragment;
            }
            else
            {
                Logging::Warning(
                    "ShaderStage: unrecognized extension '" + extension +
                    "' for '" + path.string() + "', assuming vertex stage");
                type = ShaderStage::Type::Vertex;
            }

            return std::make_shared<ShaderStage>(path, type);
        }
    };
}

#endif