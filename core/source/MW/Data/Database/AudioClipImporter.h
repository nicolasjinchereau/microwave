/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Audio/AudioClip.h>
#include <MW/Audio/AudioSample.h>
#include <MW/Data/Database/AssetImporter.h>
#include <MW/Data/Library/AssetSettings.h>
#include <MW/IO/File.h>

namespace mw {
inline namespace data {

class AudioClipImporter : public AssetImporter
{
public:
    virtual std::span<std::string> GetSupportedFileTypes() override {
        static std::string types[] = { ".mp3", ".ogg", ".wav" };
        return types;
    }

    virtual void ImportFile(
        AssetMetadata& meta,
        const sptr<Stream>& stream,
        const path& dataDir) override
    {
        AudioClipSettings settings = meta.settings;
        
        std::string ext = GetExtension(meta.sourcePath);
        if (ext == ".mp3")
            settings.fileFormat = AudioFileFormat::Mp3;
        else if (ext == ".ogg")
            settings.fileFormat = AudioFileFormat::Ogg;
        else if (ext == ".wav")
            settings.fileFormat = AudioFileFormat::Wav;
        else
            throw std::runtime_error("unsupported audio file format");

        ArtifactMetadata art;
        art.sourcePath = meta.sourcePath;
        art.uuid = !meta.artifacts.empty() ? meta.artifacts[0].uuid : UUID::New();
        art.assetType = AssetType::AudioClip;

        path artifactFilePath = dataDir / art.uuid.ToString();

        if (path::exists(artifactFilePath))
            path::remove(artifactFilePath);

        sptr<Stream> output = File::Open(artifactFilePath, OpenMode::Out | OpenMode::Binary);
        stream->CopyTo(output);
        output = nullptr;

        meta.artifacts.clear();
        meta.artifacts.push_back(std::move(art));

        meta.settings = settings;
    }
};

} // data
} // mw
