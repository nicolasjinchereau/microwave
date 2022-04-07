/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include <MW/Data/Library/AssetType.h>
#include <MW/Data/Library/AssetLoader.h>
#include <MW/Data/Library/AssetManifest.h>
#include <MW/System/Json.h>
#include <MW/System/Object.h>
#include <MW/System/path.h>
#include <MW/System/Platforms.h>
#include <MW/System/SyncExecutor.h>
#include <MW/System/UUID.h>

namespace mw {

inline namespace gfx {
class GraphicsContext;
class Texture;
class Shader;
class Material;
}

inline namespace scene {
class Node;
}

}

namespace mw {
inline namespace data {

class AssetLibrary : public Object
{
protected:
    path rootDir;
    path dataDir;

    std::unordered_map<UUID, AssetArtifact> artifacts;
    std::unordered_map<path, UUID> artifactIDs;
    std::unordered_map<AssetType, sptr<AssetLoader>> loaders;
    std::unordered_map<UUID, sptr<Object>> assets;
    std::unordered_map<UUID, Task<sptr<Object>>> assetRequests;

    sptr<AssetLoader> GetLoader(const path& sourcePath);
    void ReloadManifest();

    Task<sptr<Object>> TryGetAssetAsync(const UUID& uuid, const sptr<Executor>& executor);
public:
    AssetLibrary(const path& rootDir);
    ~AssetLibrary();

    const path& GetRootDir() const;
    const path& GetDataDir() const;

    void Refresh();

    void ReleaseUnusedAssets();
    void ReleaseAsset(const UUID& uuid);

    UUID GetAssetUUID(const path& sourcePath);
    std::optional<UUID> FindAssetUUID(const std::string& filename);

    sptr<Object> GetAsset(const UUID& uuid);
    sptr<Object> GetAsset(const path& sourcePath);
    sptr<Object> FindAsset(const std::string& filename);

    Task<sptr<Object>> GetAssetAsync(const UUID& uuid, const sptr<Executor>& executor);
    Task<sptr<Object>> GetAssetAsync(const UUID& uuid);
    Task<sptr<Object>> GetAssetAsync(const path& sourcePath);
    Task<sptr<Object>> FindAssetAsync(const std::string& filename);

    template<class T>
    sptr<T> GetAsset(const UUID& uuid);

    template<class T>
    sptr<T> GetAsset(const path& sourcePath);

    template<class T>
    sptr<T> FindAsset(const std::string& sourceFilename);

    template<class T>
    Task<sptr<T>> GetAssetAsync(const UUID& uuid);

    template<class T>
    Task<sptr<T>> GetAssetAsync(const path& sourcePath);

    template<class T>
    Task<sptr<T>> FindAssetAsync(const std::string& sourceFilename);
};

template<class T>
sptr<T> AssetLibrary::GetAsset(const UUID& uuid) {
    return std::dynamic_pointer_cast<T>(GetAsset(uuid));
}

template<class T>
sptr<T> AssetLibrary::GetAsset(const path& sourcePath) {
    return std::dynamic_pointer_cast<T>(GetAsset(sourcePath));
}

template<class T>
sptr<T> AssetLibrary::FindAsset(const std::string& sourceFilename) {
    return std::dynamic_pointer_cast<T>(FindAsset(sourceFilename));
}

template<class T>
Task<sptr<T>> AssetLibrary::GetAssetAsync(const UUID& uuid) {
    co_return std::dynamic_pointer_cast<T>(co_await GetAssetAsync(uuid));
}

template<class T>
Task<sptr<T>> AssetLibrary::GetAssetAsync(const path& sourcePath) {
    co_return std::dynamic_pointer_cast<T>(co_await GetAssetAsync(sourcePath));
}

template<class T>
Task<sptr<T>> AssetLibrary::FindAssetAsync(const std::string& sourceFilename) {
    co_return std::dynamic_pointer_cast<T>(co_await FindAssetAsync(sourceFilename));
}

}
}
