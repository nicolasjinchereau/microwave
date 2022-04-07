/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Data/Library/AssetType.h>
#include <MW/System/Object.h>
#include <MW/System/path.h>
#include <cstdint>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace mw {
inline namespace data {

class AssetImporter;
class AssetLibrary;

struct ArtifactMetadata
{
    UUID uuid;
    path sourcePath;
    AssetType assetType = {};
};

struct AssetMetadata
{
    path sourcePath;
    json settings;
    std::vector<ArtifactMetadata> artifacts;
    bool dirty = false;
};

struct ArtifactRecord
{
    uint64_t lastModified;
};

struct AssetRecord
{
    uint64_t sourceLastModified = 0;
    uint64_t metaLastModified = 0;
    std::unordered_map<UUID, ArtifactRecord> artifactRecords;
};

class AssetDatabase : public Object
{
    path rootDir;
    path sourceDir;
    path dataDir;
    std::unordered_map<std::string, sptr<AssetImporter>> importers; // by file extension
    std::unordered_map<std::string, sptr<AssetMetadata>> metadata; // by relative path
    std::unordered_map<path, AssetRecord> catalog;
    sptr<AssetLibrary> assetLibrary;

    static std::unordered_map<path, std::string_view> internalShaderAssets;
public:
    AssetDatabase(const sptr<AssetLibrary>& assetLibrary);

    const path& GetRootDir() const;
    const path& GetSourceDir() const;
    const path& GetDataDir() const;
    const sptr<AssetLibrary>& GetAssetLibrary() const;

    void SetDirty(const path& sourceFile);
    void Refresh(bool force = false);
    void Deploy(const path& dest);

    json GetImportSettings(const path& sourceFile);
    void SetImportSettings(const path& sourceFile, const json& settings);

    template<class T>
    T GetImportSettings(const path& sourceFile);

    template<class T>
    void SetImportSettings(const path& sourceFile, const T& settings);

private:
    sptr<AssetImporter> GetImporter(const path& sourceFile);

    sptr<AssetMetadata> GetAssetMetadata(const path& sourcePath);

    void ExtractInternalAssets();
    void LoadCatalog();
    void SaveCatalog();
    void UpdateMetadata();
    void ImportFiles(bool force);
    void ResolveReferences();
    void ExportManifest();
    void RemoveOrphanedImports();
    void RemoveEmptyFolders(const path& dir);
    void RemoveIfIsEmptyDir(const path& targetPath);
    void EnsureDirectoryExists(const path& dir);
    void ThrowOnInvalidPath(const path& p, const std::string& purpose);
};

template<class T>
inline T AssetDatabase::GetImportSettings(const path& sourceFile)
{
    T ret;
    json obj = GetImportSettings(sourceFile);
    from_json(obj, ret);
    return ret;
}

template<class T>
inline void AssetDatabase::SetImportSettings(const path& sourceFile, const T& settings) {
    json obj;
    to_json(obj, settings);
    SetImportSettings(sourceFile, obj);
}

inline void to_json(json& obj, const ArtifactRecord& record) {
    obj["lastModified"] = record.lastModified;
}

inline void from_json(const json& obj, ArtifactRecord& record) {
    record.lastModified = obj.value("lastModified", record.lastModified);
}

inline void to_json(json& obj, const AssetRecord& record) {
    obj["sourceLastModified"] = record.sourceLastModified;
    obj["metaLastModified"] = record.metaLastModified;
    obj["artifactRecords"] = record.artifactRecords;
}

inline void from_json(const json& obj, AssetRecord& record) {
    record.sourceLastModified = obj.value("sourceLastModified", record.sourceLastModified);
    record.metaLastModified = obj.value("metaLastModified", record.metaLastModified);
    record.artifactRecords = obj.value("artifactRecords", record.artifactRecords);
}

inline void to_json(json& obj, const ArtifactMetadata& art) {
    obj["uuid"] = art.uuid;
    obj["assetType"] = art.assetType;
    obj["sourcePath"] = art.sourcePath;
}

inline void from_json(const json& obj, ArtifactMetadata& art) {
    art.uuid = obj.value("uuid", art.uuid);
    art.assetType = obj.value("assetType", art.assetType);
    art.sourcePath = obj.value("sourcePath", art.sourcePath);
}

inline void to_json(json& obj, const AssetMetadata& meta) {
    obj["sourcePath"] = meta.sourcePath;
    obj["settings"] = meta.settings;
    obj["artifacts"] = meta.artifacts;
}

inline void from_json(const json& obj, AssetMetadata& meta)
{
    meta.sourcePath = obj.value("sourcePath", meta.sourcePath);
    meta.settings = obj.value("settings", json::object());
    meta.artifacts = obj.value("artifacts", meta.artifacts);
}

} // data
} // mw
