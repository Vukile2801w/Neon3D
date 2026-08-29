#include "Assets/AssetManager.hpp"

#include <system_error>

#include "Logging.hpp"

namespace Neon
{
    void AssetManager::clear()
    {
        m_assets.clear();

        Logging::Info("AssetManager cache cleared");
    }

    std::string AssetManager::normalizePath(
        const std::filesystem::path &path) const
    {
        std::error_code error;

        std::filesystem::path normalized =
            std::filesystem::weakly_canonical(
                path,
                error);

        if (error)
        {
            normalized =
                std::filesystem::absolute(
                    path,
                    error);

            if (error)
                normalized = path;
        }

        normalized =
            normalized.lexically_normal();

        return normalized.string();
    }

    std::string AssetManager::makeKey(
        const std::filesystem::path &path,
        const std::string &type) const
    {
        return type + ":" + normalizePath(path);
    }

    Ref<Shader> AssetManager::load(
        const Ref<ShaderStage> &vertex,
        const Ref<ShaderStage> &fragment)
    {
        if (!vertex || !fragment)
        {
            Logging::Error(
                "AssetManager: cannot load Shader from null ShaderStage");

            return nullptr;
        }

        const std::string key =
            "Shader:" +
            std::to_string(vertex->getId()) +
            ":" +
            std::to_string(fragment->getId());

        auto it = m_assets.find(key);

        if (it != m_assets.end())
        {
            return std::static_pointer_cast<Shader>(
                it->second);
        }

        Ref<Shader> shader =
            AssetLoader<Shader>::load(
                vertex,
                fragment);

        if (!shader)
            return nullptr;

        m_assets.emplace(key, shader);

        return shader;
    }
}