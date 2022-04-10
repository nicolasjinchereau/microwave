/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Data.Database.AssetDatabase;
import Microwave.Data.Database.AssetImporter;
import Microwave.Data.Database.AudioClipImporter;
import Microwave.Data.Database.DefaultImporter;
import Microwave.Data.Database.ModelImporter;
import Microwave.Data.Database.TextureImporter;
import Microwave.Data.Library.AssetLibrary;
import Microwave.System.Console;
import Microwave.System.Json;
import Microwave.Graphics.Color;
import Microwave.Graphics.Image;
import Microwave.IO.File;
import Microwave.IO.MemoryStream;
import Microwave.Utilities.Util;
import <cassert>;
import <iomanip>;
import <stdexcept>;
import <MW/Data/Internal/Assets.h>;

namespace fs = std::filesystem;

namespace mw {
inline namespace data {

AssetDatabase::AssetDatabase(const sptr<AssetLibrary>& assetLibrary)
    : assetLibrary(assetLibrary)
{
    assert(assetLibrary);
    rootDir = assetLibrary->GetRootDir();
    sourceDir = rootDir / "source";
    dataDir = assetLibrary->GetDataDir();

    sptr<AssetImporter> defaultImporter = spnew<DefaultImporter>();
    for (auto& ext : defaultImporter->GetSupportedFileTypes())
        importers[ext] = defaultImporter;

    sptr<AssetImporter> modelImporter = spnew<ModelImporter>();
    for (auto& ext : modelImporter->GetSupportedFileTypes())
        importers[ext] = modelImporter;

    sptr<AssetImporter> texImporter = spnew<TextureImporter>();
    for (auto& ext : texImporter->GetSupportedFileTypes())
        importers[ext] = texImporter;

    sptr<AssetImporter> audImporter = spnew<AudioClipImporter>();
    for (auto& ext : audImporter->GetSupportedFileTypes())
        importers[ext] = audImporter;
}

const path& AssetDatabase::GetRootDir() const {
    return rootDir;
}

const path& AssetDatabase::GetSourceDir() const {
    return sourceDir;
}

const path& AssetDatabase::GetDataDir() const {
    return dataDir;
}

const sptr<AssetLibrary>& AssetDatabase::GetAssetLibrary() const {
    return assetLibrary;
}

void AssetDatabase::SetDirty(const path& sourceFile)
{
    path p = sourceFile;
    p.make_preferred();

    auto it = metadata.find(p);
    if (it != metadata.end())
        it->second->dirty = true;
}

void AssetDatabase::Refresh(bool force)
{
    ExtractInternalAssets();
    LoadCatalog();
    UpdateMetadata();
    ImportFiles(force);
    RemoveOrphanedImports();
    SaveCatalog();
}

void AssetDatabase::ExtractInternalAssets()
{
    EnsureDirectoryExists(sourceDir / ".internal");

    for (auto& [sourcePath, text] : internalAssets)
    {
        auto fullPath = sourceDir / sourcePath;

        if (!path::exists(fullPath)) {
            File::WriteAllText(fullPath, text);
        }
    }

    {
        std::string_view whiteTexturePath = ".internal/white.png";
        auto fullTexPath = sourceDir / whiteTexturePath;

        if (!path::exists(fullTexPath))
        {
            auto pixel = ColorBGRA(255, 255, 255, 255);
            auto data = std::span<std::byte>((std::byte*)&pixel, sizeof(ColorBGRA));
            auto img = spnew<Image>(PixelDataFormat::RGBA32, IVec2(1, 1), data);

            std::vector<std::byte> fileData;
            img->SavePNG(fileData);
            File::WriteAllBytes(fullTexPath, fileData);
        }
    }
}

void AssetDatabase::LoadCatalog()
{
    path catalogPath = dataDir / "catalog.json";

    if (path::exists(catalogPath))
    {
        auto text = File::ReadAllText(catalogPath);
        catalog = json::parse(text).get<decltype(catalog)>();
    }
}

void AssetDatabase::SaveCatalog()
{
    for (auto& [sourcePath, meta] : metadata)
    {
        path fullSourcePath = sourceDir / sourcePath;
        path fullMetaPath = fullSourcePath + ".meta";

        auto& rec = catalog[sourcePath];
        rec.sourceLastModified = File::GetLastWriteTime(fullSourcePath);
        rec.metaLastModified = File::GetLastWriteTime(fullMetaPath);

        for (auto& art : meta->artifacts)
        {
            path artifactPath = dataDir / art.uuid.ToString();
            auto& artRec = rec.artifactRecords[art.uuid];
            artRec.lastModified = File::GetLastWriteTime(artifactPath);
        }
    }

    path catalogPath = dataDir / "catalog.json";
    json obj = catalog;
    File::WriteAllText(catalogPath, obj.dump(2));
}

// update in-memory metadata object cache and delete extra *.meta files from disk
void AssetDatabase::UpdateMetadata()
{
    // remove in-memory meta objects if source or meta file missing
    for (auto it = metadata.begin(); it != metadata.end(); )
    {
        path sourcePath = it->first;
        path fullSourcePath = sourceDir / sourcePath;
        path fullMetaPath = fullSourcePath + ".meta";

        // if no source file, meta not needed
        // if meta deleted, it will be regenerated
        if (!path::exists(fullSourcePath) || !path::exists(fullMetaPath))
        {
            it = metadata.erase(it);
            catalog.erase(sourcePath);
        }
        else
        {
            ++it;
        }
    }

    // catalogue source and meta files on disk
    std::vector<path> metaFilesToDelete; // meta files without source (delete)
    std::vector<path> metaFilesToCreate;  // source file without meta file (create)
    std::vector<path> metaFilesToLoad;   // meta+source exist, but meta not loaded (load)

    for (auto& entry : fs::recursive_directory_iterator(sourceDir.std_path()))
    {
        if (entry.is_directory())
            continue;
        
        path fullPath = entry.path();
        std::string ext = GetExtension(fullPath);
        
        if (ext == ".meta")
        {
            path fullSourcePath = path(fullPath).replace_extension();
            path sourcePath = path::relative(fullSourcePath, sourceDir);

            if (!path::exists(fullSourcePath)) // matching source file missing?
                metaFilesToDelete.push_back(fullPath); // delete
            else if (metadata.count(sourcePath) == 0) // meta file not loaded?
                metaFilesToLoad.push_back(fullPath); // load
        }
        else // ext != ".meta"
        {
            path metaFile = fullPath + ".meta";

            if (!path::exists(metaFile)) // matching meta file missing?
                metaFilesToCreate.push_back(fullPath); // create
        }
    }

    // delete *.meta files if matching source missing
    for (auto& metaFile : metaFilesToDelete)
    {
        std::error_code ec;
        path::remove(metaFile, ec);
    }

    // add metadata object for untracked source files
    for (auto& fullSourcePath : metaFilesToCreate)
    {
        path sourcePath = path::relative(fullSourcePath, sourceDir);

        auto meta = spnew<AssetMetadata>();
        meta->sourcePath = sourcePath;
        meta->settings = json::object();
        meta->artifacts = {};
        meta->dirty = true;

        metadata[sourcePath] = meta;
    }

    // load existing meta files from disk
    for (auto& metaFilePath : metaFilesToLoad)
    {
        path fullSourcePath = path(metaFilePath).replace_extension();
        path sourcePath = path::relative(fullSourcePath, sourceDir);

        auto meta = spnew<AssetMetadata>();
        json obj = json::parse(File::ReadAllText(metaFilePath));
        from_json(obj, *meta);
        meta->dirty = false;

        metadata[sourcePath] = meta;
    }

    // mark metadata dirty
    for (auto& [sourcePath, meta] : metadata)
    {
        if (meta->dirty)
            continue;

        path fullSourcePath = sourceDir / sourcePath;
        path fullMetaPath = fullSourcePath + ".meta";

        if (meta->sourcePath != sourcePath)
        {
            meta->dirty = true;
            continue;
        }

        // source/meta files modified?
        uint64_t metaLastModified = File::GetLastWriteTime(fullMetaPath);
        uint64_t sourceLastModified = File::GetLastWriteTime(fullSourcePath);

        auto& rec = catalog[sourcePath];

        if (rec.metaLastModified != metaLastModified ||
            rec.sourceLastModified != sourceLastModified)
        {
            meta->dirty = true;
            continue;
        }

        if (meta->artifacts.empty())
        {
            meta->dirty = true;
            continue;
        }

        for (auto& art : meta->artifacts)
        {
            auto artifactFile = dataDir / art.uuid.ToString();

            if (!path::exists(artifactFile))
            {
                meta->dirty = true;
                break;
            }
            
            uint64_t artifactLastModified = File::GetLastWriteTime(artifactFile);
            auto& artRec = rec.artifactRecords[art.uuid];

            if (artRec.lastModified != artifactLastModified)
            {
                meta->dirty = true;
                break;
            }
        }
    }
}

void AssetDatabase::ImportFiles(bool force)
{
    EnsureDirectoryExists(sourceDir);
    EnsureDirectoryExists(dataDir);

    for (auto& [sourcePath, meta] : metadata)
    {
        if (meta->dirty || force)
        {
            Console::WriteLine("Importing File: %", sourcePath);

            auto fullSourcePath = sourceDir / sourcePath;
            auto fullMetaPath = fullSourcePath + ".meta";

            meta->sourcePath = sourcePath;
            
            auto fileStream = File::Open(fullSourcePath, OpenMode::In | OpenMode::Binary);
            auto importer = GetImporter(sourcePath);
            importer->ImportFile(*meta, fileStream, dataDir);

            // save meta file
            json obj;
            to_json(obj, *meta);

            File::WriteAllText(fullMetaPath, obj.dump(2));
            meta->dirty = false;

            for(auto& art : meta->artifacts)
                assetLibrary->ReleaseAsset(art.uuid);
        }
    }

    ExportManifest();
    assetLibrary->Refresh();

    ResolveReferences();
}

void AssetDatabase::ResolveReferences()
{
    // once all assets imported, run second pass to resolved references
    for (auto& [sourcePath, meta] : metadata)
    {
        auto importer = GetImporter(sourcePath);
        bool resolved = importer->Resolve(*this, *meta, sourceDir, dataDir);
        if (resolved)
        {
            auto fullSourcePath = sourceDir / sourcePath;
            auto fullMetaPath = fullSourcePath + ".meta";

            // save meta file
            json obj;
            to_json(obj, *meta);
            File::WriteAllText(fullMetaPath, obj.dump(2));

            for (auto& art : meta->artifacts)
                assetLibrary->ReleaseAsset(art.uuid);
        }
    }

    assetLibrary->Refresh();
}

void AssetDatabase::ExportManifest()
{
    EnsureDirectoryExists(dataDir);

    AssetManifest manifest;

    for (auto& [relativeSourcePath, meta] : metadata)
    {
        for (auto& artMeta : meta->artifacts)
        {
            AssetArtifact art;
            art.uuid = artMeta.uuid;
            art.sourcePath = artMeta.sourcePath;
            art.assetType = artMeta.assetType;
            art.settings = meta->settings;

            manifest.artifacts.push_back(art);
        }
    }

    json obj = manifest;
    File::WriteAllText(dataDir / "manifest.json", obj.dump(2));
}

void AssetDatabase::RemoveOrphanedImports()
{
    std::unordered_set<UUID> validIDs;

    for (auto& [sourceFile, meta] : metadata)
    {
        for (auto& art : meta->artifacts)
            validIDs.insert(art.uuid);
    }

    for (auto& entry : fs::recursive_directory_iterator(dataDir.string()))
    {
        if (!entry.is_directory())
        {
            auto idStr = entry.path().filename().string();
            auto uuid = UUID::FromString(idStr);
            if (uuid && !validIDs.count(uuid))
            {
                std::error_code ec;
                path::remove(entry.path(), ec);
            }
        }
    }
}

void AssetDatabase::RemoveEmptyFolders(const path& dir)
{
    if (!path::exists(dir))
        throw std::runtime_error("path does not exist: " + dir);

    std::vector<path> children;

    for (auto& entry : fs::directory_iterator(dir.native()))
    {
        if (entry.is_directory())
            children.push_back(entry.path());
    }

    for (auto& dir : children)
        RemoveIfIsEmptyDir(dir);
}

void AssetDatabase::RemoveIfIsEmptyDir(const path& targetPath)
{
    if (path::is_directory(targetPath))
    {
        // remove empty children of 'path' first
        std::vector<path> children;

        for (auto& entry : fs::directory_iterator(targetPath.string()))
        {
            if (entry.is_directory())
                children.push_back(entry.path());
        }

        for (auto& dir : children) {
            RemoveIfIsEmptyDir(dir);
        }

        // now remove 'path' itself, if it's empty
        if (path::is_empty(targetPath)) {
            path::remove(targetPath);
        }
    }
}

sptr<AssetImporter> AssetDatabase::GetImporter(const path& sourceFile)
{
    sptr<AssetImporter> importer;

    auto ext = ToLower(sourceFile.extension().string());

    if (auto it = importers.find(ext);
        it != importers.end())
    {
        importer = it->second;
    }
    else
    {
        importer = importers[".*"];
    }

    return importer;
}

void AssetDatabase::Deploy(const path& dest)
{
    EnsureDirectoryExists(dest);

    path destDataDir = dest / "data";
    EnsureDirectoryExists(destDataDir);

    AssetManifest manifest;

    for (auto& [source, meta] : metadata)
    {
        for (auto& artMeta : meta->artifacts)
        {
            AssetArtifact art;
            art.uuid = artMeta.uuid;
            art.sourcePath = artMeta.sourcePath;
            art.assetType = artMeta.assetType;
            art.settings = meta->settings;

            manifest.artifacts.push_back(art);

            std::string uuid = art.uuid.ToString();
            path from = dataDir / uuid;
            path to = destDataDir / uuid;

            path::copy(
                from.native(),
                to.native(),
                path::copy_options::overwrite_existing);
        }
    }

    json obj = manifest;

    auto manifestPath = dest / "manifest.json";
    auto manifestText = obj.dump(2);
    auto manifestData = std::span<std::byte>((std::byte*)manifestText.data(), manifestText.size());
    File::WriteAllBytes(manifestPath, manifestData);
}

sptr<AssetMetadata> AssetDatabase::GetAssetMetadata(const path& sourcePath)
{
    sptr<AssetMetadata> ret;

    auto it = metadata.find(sourcePath);
    if (it != metadata.end())
        ret = it->second;

    return ret;
}

json AssetDatabase::GetImportSettings(const path& sourceFile)
{
    auto meta = GetAssetMetadata(sourceFile);
    if (!meta)
        throw std::runtime_error("no metadata found for specified source file");

    return meta->settings;
}

void AssetDatabase::SetImportSettings(const path& sourceFile, const json& settings)
{
    auto meta = GetAssetMetadata(sourceFile);
    if (!meta)
        throw std::runtime_error("no metadata found for specified source file");

    meta->settings = settings;
    meta->dirty = true;
}

void AssetDatabase::EnsureDirectoryExists(const path& dir)
{
    if (!path::exists(dir))
    {
        path::create_directories(dir);
    }
    else if (!path::is_directory(dir))
    {
        throw std::runtime_error("specified path already exists and is not a directory");
    }
}

void AssetDatabase::ThrowOnInvalidPath(const path& p, const std::string& purpose)
{
    if (p.empty() || !path::exists(p))
        throw std::runtime_error(
            "the path specified for '" + purpose + "' does not exist: '" + p + "'");
}

}
}
