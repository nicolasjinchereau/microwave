/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Data/Database/AssetDatabase.h>
#include <MW/Data/Database/AssetImporter.h>
#include <MW/System/Json.h>
#include <MW/IO/File.h>
#include <MW/System/path.h>
#include <MW/System/Platforms.h>
#include <MW/System/UUID.h>
#include <filesystem>
#include <fstream>

namespace mw {
inline namespace data {

class AssetDatabase;

class DefaultImporter : public AssetImporter
{
public:
    virtual std::span<std::string> GetSupportedFileTypes() override {
        static std::string types[] = { ".*" };
        return types;
    }

    virtual void ImportFile(
        AssetMetadata& meta,
        const sptr<Stream>& stream,
        const path& dataDir) override
    {
        ArtifactMetadata art;
        art.sourcePath = meta.sourcePath;
        art.uuid = !meta.artifacts.empty() ? meta.artifacts[0].uuid : UUID::New();
        
        std::string ext = GetExtension(meta.sourcePath);
        if (ext == ".cg") {
            art.assetType = AssetType::Shader;
        }
        else if (ext == ".ttf") {
            art.assetType = AssetType::Font;
        }
        else if (ext == ".mesh") {
            art.assetType = AssetType::Mesh;
        }
        else if (ext == ".mat") {
            art.assetType = AssetType::Material;
        }
        else {
            art.assetType = AssetType::Binary;
        }

        path artifactFilePath = dataDir / art.uuid.ToString();

        if (path::exists(artifactFilePath))
            path::remove(artifactFilePath);

        sptr<Stream> output = File::Open(artifactFilePath, OpenMode::Out | OpenMode::Binary);
        stream->CopyTo(output);
        output = nullptr;
        
        meta.artifacts.clear();
        meta.artifacts.push_back(std::move(art));
    }
};

}
}
