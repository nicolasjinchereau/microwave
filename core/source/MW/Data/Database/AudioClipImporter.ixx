/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Data.Database.AudioClipImporter;
import Microwave.Audio.AudioClip;
import Microwave.Audio.AudioSample;
import Microwave.Data.Database.AssetDatabase;
import Microwave.Data.Database.AssetImporter;
import Microwave.Data.Library.AssetSettings;
import Microwave.IO.File;
import Microwave.IO.Stream;
import Microwave.System.Exception;
import Microwave.System.Json;
import Microwave.System.Path;
import Microwave.System.Pointers;
import Microwave.System.UUID;
import Microwave.Utilities.Util;
import <span>;
import <string>;

export namespace mw {
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
        const gptr<Stream>& stream,
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
            throw Exception("unsupported audio file format");

        ArtifactMetadata art;
        art.sourcePath = meta.sourcePath;
        art.uuid = !meta.artifacts.empty() ? meta.artifacts[0].uuid : UUID::New();
        art.assetType = AssetType::AudioClip;

        path artifactFilePath = dataDir / art.uuid.ToString();

        if (path::exists(artifactFilePath))
            path::remove(artifactFilePath);

        gptr<Stream> output = File::Open(artifactFilePath, OpenMode::Out | OpenMode::Binary);
        stream->CopyTo(output);
        output = nullptr;

        meta.artifacts.clear();
        meta.artifacts.push_back(std::move(art));

        meta.settings = settings;
    }
};

} // data
} // mw
