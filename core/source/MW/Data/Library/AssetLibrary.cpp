/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/Data/Library/AssetLibrary.h>
#include <MW/Data/Library/AssetSettings.h>
#include <MW/Data/Library/AudioClipLoader.h>
#include <MW/Data/Library/BinaryLoader.h>
#include <MW/Data/Library/ObjectLoader.h>
#include <MW/Data/Library/ShaderLoader.h>
#include <MW/Data/Library/TextureLoader.h>
#include <MW/Data/Library/FontLoader.h>
#include <MW/System/Json.h>
#include <MW/Graphics/GraphicsContext.h>
#include <MW/Graphics/Image.h>
#include <MW/Graphics/Material.h>
#include <MW/Graphics/Model.h>
#include <MW/Graphics/Shader.h>
#include <MW/Graphics/Texture.h>
#include <MW/IO/File.h>
#include <MW/IO/FileStream.h>
#include <MW/Scene/Node.h>
#include <algorithm>

namespace mw {
inline namespace data {

AssetLibrary::AssetLibrary(const path& rootDir)
    : rootDir(rootDir)
{
    dataDir = rootDir / "data";

    auto objectLoader = spnew<ObjectLoader>();

    loaders[AssetType::AnimationClip] = objectLoader;
    loaders[AssetType::AudioClip] = spnew<AudioClipLoader>();
    loaders[AssetType::Binary] = spnew<BinaryLoader>();
    loaders[AssetType::Font] = spnew<FontLoader>();
    loaders[AssetType::Material] = objectLoader;
    loaders[AssetType::Mesh] = objectLoader;
    loaders[AssetType::Node] = objectLoader;
    loaders[AssetType::Shader] = spnew<ShaderLoader>();
    loaders[AssetType::Texture] = spnew<TextureLoader>();

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

sptr<AssetLoader> AssetLibrary::GetLoader(const path& sourcePath)
{
    sptr<AssetLoader> ret;

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

Task<sptr<Object>> AssetLibrary::GetAssetAsync(const UUID& uuid, const sptr<Executor>& executor)
{
    sptr<Object> ret;

    auto it = assetRequests.find(uuid);
    if (it != assetRequests.end())
    {
        Task<sptr<Object>> request = it->second;
        ret = co_await request;
    }
    else
    {
        Task<sptr<Object>> request = TryGetAssetAsync(uuid, executor);
        assetRequests[uuid] = request;
        ret = co_await request;
        assetRequests.erase(uuid);
    }

    co_return ret;
}

Task<sptr<Object>> AssetLibrary::TryGetAssetAsync(const UUID& uuid, const sptr<Executor>& executor)
{
    sptr<Object> ret;

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
                assert(loader);

                Console::WriteLine("Loading: %", art.sourcePath.string());

                ret = co_await loader->LoadAsync(filePath, art, executor);
                if (ret)
                    assets[uuid] = ret;
            }
        }
    }

    co_return ret;
}

sptr<Object> AssetLibrary::GetAsset(const UUID& uuid) {
    return GetAssetAsync(uuid, SyncExecutor::GetInstance()).GetResult();
}

sptr<Object> AssetLibrary::GetAsset(const path& sourcePath) {
    return GetAsset(GetAssetUUID(sourcePath));
}

sptr<Object> AssetLibrary::FindAsset(const std::string& filename) {
    auto uuid = FindAssetUUID(filename);
    return uuid ? GetAsset(*uuid) : sptr<Object>{};
}

Task<sptr<Object>> AssetLibrary::GetAssetAsync(const UUID& uuid) {
    sptr<Object> ret = co_await GetAssetAsync(uuid, ThreadPool::GetInstance());
    co_return ret;
}

Task<sptr<Object>> AssetLibrary::FindAssetAsync(const std::string& filename)
{
    sptr<Object> ret;
    auto uuid = FindAssetUUID(filename);
    if(uuid) ret = co_await GetAssetAsync(*uuid);
    co_return ret;
}

Task<sptr<Object>> AssetLibrary::GetAssetAsync(const path& sourcePath) {
    auto uuid = GetAssetUUID(sourcePath);
    sptr<Object> ret = co_await GetAssetAsync(uuid);
    co_return ret;
}

void AssetLibrary::ReleaseUnusedAssets()
{
    for (auto it = assets.begin(); it != assets.end(); )
    {
        if (it->second.use_count() == 1)
            it = assets.erase(it);
        else
            ++it;
    }
}

void AssetLibrary::ReleaseAsset(const UUID& uuid) {
    assets.erase(uuid);
}

}
}
