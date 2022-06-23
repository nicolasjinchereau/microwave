/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Data.Library.AssetLibrary;
import Microwave.Data.Library.AssetSettings;
import Microwave.Data.Library.AudioClipLoader;
import Microwave.Data.Library.BinaryLoader;
import Microwave.Data.Library.ObjectLoader;
import Microwave.Data.Library.ShaderLoader;
import Microwave.Data.Library.TextureLoader;
import Microwave.Data.Library.FontLoader;
import Microwave.Graphics.GraphicsContext;
import Microwave.Graphics.Image;
import Microwave.Graphics.Material;
import Microwave.Graphics.Model;
import Microwave.Graphics.Shader;
import Microwave.Graphics.Texture;
import Microwave.IO.File;
import Microwave.IO.FileStream;
import Microwave.IO.Terminal;
import Microwave.System.Exception;
import Microwave.System.Executor;
import Microwave.System.Json;
import Microwave.System.Object;
import Microwave.System.Path;
import Microwave.System.Pointers;
import Microwave.System.SyncExecutor;
import Microwave.System.Task;
import Microwave.System.ThreadPool;
import Microwave.System.UUID;
import Microwave.SceneGraph.Node;
import <MW/System/Debug.h>;
import <algorithm>;
import <memory>;
import <optional>;
import <string>;
import <unordered_map>;

namespace mw {
inline namespace data {

AssetLibrary::AssetLibrary(const path& rootDir)
    : rootDir(rootDir)
{
    dataDir = rootDir / "data";

    auto objectLoader = gpnew<ObjectLoader>();

    loaders[AssetType::AnimationClip] = objectLoader;
    loaders[AssetType::AudioClip] = gpnew<AudioClipLoader>();
    loaders[AssetType::Binary] = gpnew<BinaryLoader>();
    loaders[AssetType::Font] = gpnew<FontLoader>();
    loaders[AssetType::Material] = objectLoader;
    loaders[AssetType::Mesh] = objectLoader;
    loaders[AssetType::Node] = objectLoader;
    loaders[AssetType::Shader] = gpnew<ShaderLoader>();
    loaders[AssetType::Texture] = gpnew<TextureLoader>();

    ReloadManifest();
}

AssetLibrary::~AssetLibrary()
{
}

void AssetLibrary::ReloadManifest()
{
    auto manifestPath = dataDir / "manifest.json";
    if (!path::exists(manifestPath))
        return;

    auto manifestText = File::ReadAllText(manifestPath);
    AssetManifest manifest = json::parse(manifestText);

    artifacts.clear();
    artifactIDs.clear();

    for (auto& art : manifest.artifacts)
    {
        artifacts[art.uuid] = art;
        artifactIDs[art.sourcePath] = art.uuid;
    }
}

const path& AssetLibrary::GetRootDir() const {
    return rootDir;
}

const path& AssetLibrary::GetDataDir() const {
    return dataDir;
}

void AssetLibrary::Refresh()
{
    ReloadManifest();
    
    for (auto it = assets.begin(); it != assets.end(); )
    {
        if (artifacts.find(it->first) == artifacts.end())
            it = assets.erase(it);
        else
            ++it;
    }
}

gptr<AssetLoader> AssetLibrary::GetLoader(const path& sourcePath)
{
    gptr<AssetLoader> ret;

    auto it = artifactIDs.find(sourcePath);
    if (it != artifactIDs.end())
    {
        auto& art = artifacts[it->second];
        ret = loaders[art.assetType];
    }

    return ret;
}

UUID AssetLibrary::GetAssetUUID(const path& sourcePath)
{
    auto it = artifactIDs.find(sourcePath);
    return it != artifactIDs.end() ? it->second : UUID();
}

std::optional<UUID> AssetLibrary::FindAssetUUID(const std::string& filename)
{
    std::optional<UUID> ret;

    for (auto& [sourcePath, id] : artifactIDs)
    {
        if (sourcePath.filename() == filename)
        {
            ret = id;
            break;
        }
    }

    return ret;
}

Task<gptr<Object>> AssetLibrary::GetAssetAsync(const UUID& uuid, const gptr<Executor>& executor)
{
    gptr<Object> ret;

    auto it = assetRequests.find(uuid);
    if (it != assetRequests.end())
    {
        Task<gptr<Object>> request = it->second;
        ret = co_await request;
    }
    else
    {
        Task<gptr<Object>> request = TryGetAssetAsync(uuid, executor);
        assetRequests[uuid] = request;
        ret = co_await request;
        assetRequests.erase(uuid);
    }

    co_return ret;
}

Task<gptr<Object>> AssetLibrary::TryGetAssetAsync(const UUID& uuid, const gptr<Executor>& executor)
{
    gptr<Object> ret;

    if (!uuid.empty())
    {
        if (auto it = assets.find(uuid);  it != assets.end())
        {
            ret = it->second;
        }
        else
        {
            if (auto it = artifacts.find(uuid); it != artifacts.end())
            {
                AssetArtifact& art = it->second;
                path filePath = dataDir / uuid.ToString();

                auto loader = GetLoader(art.sourcePath);
                Assert(loader);

                writeln("Loading: ", art.sourcePath.string());

                ret = co_await loader->LoadAsync(filePath, art, executor);
                if (ret)
                    assets[uuid] = ret;
            }
        }
    }

    co_return ret;
}

gptr<Object> AssetLibrary::GetAsset(const UUID& uuid) {
    return GetAssetAsync(uuid, SyncExecutor::GetInstance()).GetResult();
}

gptr<Object> AssetLibrary::GetAsset(const path& sourcePath) {
    return GetAsset(GetAssetUUID(sourcePath));
}

gptr<Object> AssetLibrary::FindAsset(const std::string& filename) {
    auto uuid = FindAssetUUID(filename);
    return uuid ? GetAsset(*uuid) : gptr<Object>{};
}

Task<gptr<Object>> AssetLibrary::GetAssetAsync(const UUID& uuid) {
    gptr<Object> ret = co_await GetAssetAsync(uuid, ThreadPool::GetInstance());
    co_return ret;
}

Task<gptr<Object>> AssetLibrary::FindAssetAsync(const std::string& filename)
{
    gptr<Object> ret;
    auto uuid = FindAssetUUID(filename);
    if(uuid) ret = co_await GetAssetAsync(*uuid);
    co_return ret;
}

Task<gptr<Object>> AssetLibrary::GetAssetAsync(const path& sourcePath) {
    auto uuid = GetAssetUUID(sourcePath);
    gptr<Object> ret = co_await GetAssetAsync(uuid);
    co_return ret;
}

void AssetLibrary::ReleaseUnusedAssets()
{
    //for (auto it = assets.begin(); it != assets.end(); )
    //{
    //    if (it->second.use_count() == 1)
    //        it = assets.erase(it);
    //    else
    //        ++it;
    //}
}

void AssetLibrary::ReleaseAsset(const UUID& uuid) {
    assets.erase(uuid);
}

void AssetLibrary::GetAllTextures(gvector<gptr<Texture>>& textures)
{
    textures.clear();

    for (auto& [uuid, obj] : assets)
    {
        auto tex = gpcast<Texture>(obj);
        if(tex)
            textures.push_back(std::move(tex));
    }
}

} // data
} // mw
