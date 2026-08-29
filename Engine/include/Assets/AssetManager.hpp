#ifndef NEON_ASSET_MANAGER
#define NEON_ASSET_MANAGER

#include <filesystem>
#include <string>
#include <unordered_map>

#include "Assets/AssetLoader.hpp"
#include "Ref.hpp"
#include "Rendering/Shader.hpp"

namespace Neon
{
    class AssetManager
    {
    public:
        AssetManager() = default;
        ~AssetManager() = default;

        template <typename T, typename... Args>
        Ref<T> load(
            const std::filesystem::path &path,
            Args &&...args);

        template <typename T>
        Ref<T> get(
            const std::filesystem::path &path) const;

        Ref<Shader> load(
            const Ref<ShaderStage> &vertex,
            const Ref<ShaderStage> &fragment);

        void clear();

    private:
        std::string normalizePath(
            const std::filesystem::path &path) const;

        std::string makeKey(
            const std::filesystem::path &path,
            const std::string &type) const;

    private:
        std::unordered_map<
            std::string,
            Ref<void>>
            m_assets;
    };
}

#include "Assets/AssetManager.inl"

#endif