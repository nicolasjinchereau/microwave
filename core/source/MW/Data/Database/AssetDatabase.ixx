/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Data.Database.AssetDatabase;
import Microwave.Data.Database.Metadata;
import Microwave.Data.Library.AssetType;
import Microwave.System.Object;
import Microwave.System.Json;
import Microwave.System.Path;
import Microwave.System.Pointers;
import Microwave.System.UUID;
import <cstdint>;
import <filesystem>;
import <string>;
import <vector>;
import <unordered_map>;
import <unordered_set>;

export namespace mw {
inline namespace data {

class AssetImporter;
class AssetLibrary;

class AssetDatabase : public Object
{
    path rootDir;
    path sourceDir;
    path dataDir;
    gmap<std::string, gptr<AssetImporter>> importers; // by file extension
    gmap<std::string, gptr<AssetMetadata>> metadata; // by relative path
    std::unordered_map<path, AssetRecord> catalog;
    gptr<AssetLibrary> assetLibrary;

public:
    AssetDatabase(const gptr<AssetLibrary>& assetLibrary);

    const path& GetRootDir() const;
    const path& GetSourceDir() const;
    const path& GetDataDir() const;
    const gptr<AssetLibrary>& GetAssetLibrary() const;

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
    gptr<AssetImporter> GetImporter(const path& sourceFile);

    gptr<AssetMetadata> GetAssetMetadata(const path& sourcePath);

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
T AssetDatabase::GetImportSettings(const path& sourceFile)
{
    T ret;
    json obj = GetImportSettings(sourceFile);
    from_json(obj, ret);
    return ret;
}

template<class T>
void AssetDatabase::SetImportSettings(const path& sourceFile, const T& settings) {
    json obj;
    to_json(obj, settings);
    SetImportSettings(sourceFile, obj);
}

} // data
} // mw
