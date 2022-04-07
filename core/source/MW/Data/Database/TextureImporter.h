/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Data/Database/AssetImporter.h>
#include <MW/Data/Library/AssetSettings.h>
#include <MW/System/Json.h>
#include <MW/Graphics/Image.h>
#include <MW/Graphics/Texture.h>
#include <MW/IO/File.h>
#include <MW/System/path.h>
#include <MW/System/Platforms.h>
#include <MW/System/UUID.h>
#include <MW/Utilities/Util.h>
#include <filesystem>
#include <fstream>

namespace mw {
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
        const sptr<Stream>& stream,
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

        sptr<Stream> output = File::Open(artifactFilePath, OpenMode::Out | OpenMode::Binary);
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
