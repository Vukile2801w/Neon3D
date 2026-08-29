#ifndef NEON_ASSET_MANAGER_INL
#define NEON_ASSET_MANAGER_INL

#include <typeinfo>
#include <utility>

namespace Neon
{
    template <typename T, typename... Args>
    Ref<T> AssetManager::load(
        const std::filesystem::path &path,
        Args &&...args)
    {
        const std::string key =
            makeKey(
                path,
                typeid(T).name());

        auto it = m_assets.find(key);

        if (it != m_assets.end())
        {
            return std::static_pointer_cast<T>(
                it->second);
        }

        Ref<T> asset =
            AssetLoader<T>::load(
                path,
                std::forward<Args>(args)...);

        if (!asset)
            return nullptr;

        m_assets.emplace(
            key,
            asset);

        return asset;
    }

    template <typename T>
    Ref<T> AssetManager::get(
        const std::filesystem::path &path) const
    {
        const std::string key =
            makeKey(
                path,
                typeid(T).name());

        auto it = m_assets.find(key);

        if (it == m_assets.end())
            return nullptr;

        return std::static_pointer_cast<T>(
            it->second);
    }
}

#endif