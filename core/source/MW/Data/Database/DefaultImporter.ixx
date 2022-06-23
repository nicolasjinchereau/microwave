/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Data.Database.DefaultImporter;
import Microwave.Data.Database.AssetDatabase;
import Microwave.Data.Database.AssetImporter;
import Microwave.Data.Library.AssetSettings;
import Microwave.IO.File;
import Microwave.IO.Stream;
import Microwave.System.Json;
import Microwave.System.Path;
import Microwave.System.Pointers;
import Microwave.System.UUID;
import Microwave.Utilities.Util;
import <span>;
import <string>;

export namespace mw {
inline namespace data {

class DefaultImporter : public AssetImporter
{
public:
    virtual std::span<std::string> GetSupportedFileTypes() override {
        static std::string types[] = { ".*" };
        return types;
    }

    virtual void ImportFile(
        AssetMetadata& meta,
        const gptr<Stream>& stream,
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

        gptr<Stream> output = File::Open(artifactFilePath, OpenMode::Out | OpenMode::Binary);
        stream->CopyTo(output);
        output = nullptr;
        
        meta.artifacts.clear();
        meta.artifacts.push_back(std::move(art));
    }
};

}
}
