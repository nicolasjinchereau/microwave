/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Data.Database.TextureImporter;
import Microwave.Data.Database.AssetDatabase;
import Microwave.Data.Database.AssetImporter;
import Microwave.Data.Library.AssetSettings;
import Microwave.Graphics.Image;
import Microwave.Graphics.Texture;
import Microwave.IO.File;
import Microwave.IO.Stream;
import Microwave.System.Json;
import Microwave.System.Path;
import Microwave.System.Pointers;
import Microwave.System.UUID;
import Microwave.Utilities.Util;
import <filesystem>;
import <fstream>;
import <span>;
import <string>;

export namespace mw {
inline namespace data {

class AssetDatabase;

class TextureImporter : public AssetImporter
{
public:
    virtual std::span<std::string> GetSupportedFileTypes() override {
        static std::string types[] = { ".exr", ".jpg", ".png", ".tga" };
        return types;
    }

    virtual void ImportFile(
        AssetMetadata& meta,
        const gptr<Stream>& stream,
        const path& dataDir) override
    {
        TextureSettings settings = meta.settings;
        
        //Image img(sourceFilePath);
        //std::vector<std::byte> output;
        //img.Save(settings.fileFormat, output);
        //File::WriteAllBytes(destFilePath, output);

        // hack this for now until Image properly handles saving all types
        settings.fileFormat = Image::GetFormatForFile(meta.sourcePath);

        ArtifactMetadata art;
        art.sourcePath = meta.sourcePath; 
        art.uuid = !meta.artifacts.empty() ? meta.artifacts[0].uuid : UUID::New();
        art.assetType = AssetType::Texture;

        //path sourceFilePath = sourceDir / meta.sourceFile;
        path artifactFilePath = dataDir / art.uuid.ToString();

        if (path::exists(artifactFilePath))
            path::remove(artifactFilePath);

        gptr<Stream> output = File::Open(artifactFilePath, OpenMode::Out | OpenMode::Binary);
        stream->CopyTo(output);
        output = nullptr;

        //auto opt = path::copy_options::overwrite_existing;
        //path::copy(sourceFilePath, artifactFilePath, opt);

        meta.artifacts.clear();
        meta.artifacts.push_back(std::move(art));

        meta.settings = settings;
    }
};

} // data
} // mw
