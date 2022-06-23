/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Data.Library.AssetLibrary;
import Microwave.Data.Library.AssetType;
import Microwave.Data.Library.AssetLoader;
import Microwave.Data.Library.AssetManifest;
import Microwave.System.Executor;
import Microwave.System.Json;
import Microwave.System.Object;
import Microwave.System.Path;
import Microwave.System.Pointers;
import Microwave.System.SyncExecutor;
import Microwave.System.Task;
import Microwave.System.UUID;
import <memory>;
import <optional>;
import <string>;
import <unordered_map>;

export namespace mw {

inline namespace gfx {
class GraphicsContext;
class Texture;
class Shader;
class Material;
}

inline namespace scene {
class Node;
}

inline namespace data {

class AssetLibrary : public Object
{
protected:
    path rootDir;
    path dataDir;

    std::unordered_map<UUID, AssetArtifact> artifacts;
    std::unordered_map<path, UUID> artifactIDs;
    gmap<AssetType, gptr<AssetLoader>> loaders;
    gmap<UUID, gptr<Object>> assets;
    gmap<UUID, Task<gptr<Object>>> assetRequests;

    gptr<AssetLoader> GetLoader(const path& sourcePath);
    void ReloadManifest();

    Task<gptr<Object>> TryGetAssetAsync(const UUID& uuid, const gptr<Executor>& executor);
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

    gptr<Object> GetAsset(const UUID& uuid);
    gptr<Object> GetAsset(const path& sourcePath);
    gptr<Object> FindAsset(const std::string& filename);

    Task<gptr<Object>> GetAssetAsync(const UUID& uuid, const gptr<Executor>& executor);
    Task<gptr<Object>> GetAssetAsync(const UUID& uuid);
    Task<gptr<Object>> GetAssetAsync(const path& sourcePath);
    Task<gptr<Object>> FindAssetAsync(const std::string& filename);

    template<class T>
    gptr<T> GetAsset(const UUID& uuid);

    template<class T>
    gptr<T> GetAsset(const path& sourcePath);

    template<class T>
    gptr<T> FindAsset(const std::string& sourceFilename);

    template<class T>
    Task<gptr<T>> GetAssetAsync(const UUID& uuid);

    template<class T>
    Task<gptr<T>> GetAssetAsync(const path& sourcePath);

    template<class T>
    Task<gptr<T>> FindAssetAsync(const std::string& sourceFilename);

    void GetAllTextures(gvector<gptr<Texture>>& textures);
};

template<class T>
gptr<T> AssetLibrary::GetAsset(const UUID& uuid) {
    return gpcast<T>(GetAsset(uuid));
}

template<class T>
gptr<T> AssetLibrary::GetAsset(const path& sourcePath) {
    return gpcast<T>(GetAsset(sourcePath));
}

template<class T>
gptr<T> AssetLibrary::FindAsset(const std::string& sourceFilename) {
    return gpcast<T>(FindAsset(sourceFilename));
}

template<class T>
Task<gptr<T>> AssetLibrary::GetAssetAsync(const UUID& uuid) {
    co_return gpcast<T>(co_await GetAssetAsync(uuid));
}

template<class T>
Task<gptr<T>> AssetLibrary::GetAssetAsync(const path& sourcePath) {
    co_return gpcast<T>(co_await GetAssetAsync(sourcePath));
}

template<class T>
Task<gptr<T>> AssetLibrary::FindAssetAsync(const std::string& sourceFilename) {
    co_return gpcast<T>(co_await FindAssetAsync(sourceFilename));
}

} // data
} // mw
