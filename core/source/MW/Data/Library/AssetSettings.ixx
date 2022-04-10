/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Data.Library.AssetSettings;
import Microwave.Audio.AudioClip;
import Microwave.Graphics.AnimationClip;
import Microwave.Graphics.Font;
import Microwave.Graphics.Image;
import Microwave.Graphics.Material;
import Microwave.Graphics.Texture;
import Microwave.Math;
import Microwave.System.Path;
import Microwave.System.Pointers;
import Microwave.System.Json;
import Microwave.System.UUID;
import <array>;
import <string>;
import <optional>;
import <unordered_map>;
import <vector>;

export namespace mw {
inline namespace data {

struct TextureSettings
{
    ImageFileFormat fileFormat = ImageFileFormat::PNG;
    WrapMode wrapMode = WrapMode::Repeat;
    FilterMode filterMode = FilterMode::Bilinear;
};

struct AudioClipSettings
{
    AudioFileFormat fileFormat = AudioFileFormat::Wav;
    bool streamFromDisk = false;
};

struct ClipSpec
{
    std::string name;
    int start = 0; // inclusive
    int stop = 0; // inclusive
    AnimationWrapMode wrapMode = AnimationWrapMode::Loop;
};

struct AssetBinding
{
    std::string filename;
    std::optional<UUID> uuid;
};

struct MaterialSettings
{
    AssetBinding shaderBinding;
    std::unordered_map<std::string, AssetBinding> textureBindings;
};

struct ModelSettings
{
    float scaleFactor = 1.0f;
    bool keepRootTransform = false;
    std::vector<ClipSpec> clipSpecs;
    std::unordered_map<UUID, MaterialSettings> materialSettings;
};

struct FontSettings
{
    IVec2 atlasSize = IVec2(256, 256);
    FontMode fontMode = FontMode::Normal;
    int spacesPerTab = 4;
    int padding = 2;
    int margin = 2;
};

void to_json(json& obj, const TextureSettings& settings) {
    obj["fileFormat"] = settings.fileFormat;
    obj["wrapMode"] = settings.wrapMode;
    obj["filterMode"] = settings.filterMode;
}

void from_json(const json& obj, TextureSettings& settings) {
    settings.fileFormat = obj.value("fileFormat", settings.fileFormat);
    settings.wrapMode = obj.value("wrapMode", settings.wrapMode);
    settings.filterMode = obj.value("filterMode", settings.filterMode);
}

void to_json(json& obj, const AudioClipSettings& settings) {
    obj["fileFormat"] = settings.fileFormat;
    obj["streamFromDisk"] = settings.streamFromDisk;
}

void from_json(const json& obj, AudioClipSettings& settings) {
    settings.fileFormat = obj.value("fileFormat", settings.fileFormat);
    settings.streamFromDisk = obj.value("streamFromDisk", settings.streamFromDisk);
}

void to_json(json& obj, const ClipSpec& spec) {
    obj["name"] = spec.name;
    obj["start"] = spec.start;
    obj["stop"] = spec.stop;
    obj["wrapMode"] = spec.wrapMode;
}

void from_json(const json& obj, ClipSpec& spec) {
    spec.name = obj.value("name", spec.name);
    spec.start = obj.value("start", spec.start);
    spec.stop = obj.value("stop", spec.stop);
    spec.wrapMode = obj.value("wrapMode", spec.wrapMode);
}

void to_json(json& obj, const AssetBinding& binding) {
    obj["filename"] = binding.filename;
    obj["uuid"] = binding.uuid;
}

void from_json(const json& obj, AssetBinding& binding) {
    binding.filename = obj.value("filename", binding.filename);
    binding.uuid = obj.value("uuid", binding.uuid);
}

void to_json(json& obj, const MaterialSettings& settings) {
    obj["shaderBinding"] = settings.shaderBinding;
    obj["textureBindings"] = settings.textureBindings;
}

void from_json(const json& obj, MaterialSettings& settings) {
    settings.shaderBinding = obj.value("shaderBinding", settings.shaderBinding);
    settings.textureBindings = obj.value("textureBindings", settings.textureBindings);
}

void to_json(json& obj, const ModelSettings& settings) {
    obj["scaleFactor"] = settings.scaleFactor;
    obj["keepRootTransform"] = settings.keepRootTransform;
    obj["clipSpecs"] = settings.clipSpecs;
    obj["materialSettings"] = settings.materialSettings;
}

void from_json(const json& obj, ModelSettings& settings) {
    settings.scaleFactor = obj.value("scaleFactor", settings.scaleFactor);
    settings.keepRootTransform = obj.value("keepRootTransform", settings.keepRootTransform);
    settings.clipSpecs = obj.value("clipSpecs", settings.clipSpecs);
    settings.materialSettings = obj.value("materialSettings", settings.materialSettings);
}

void to_json(json& obj, const FontMode& mode) {
    static std::array<const char*, 2> modes{
        "Normal",
        "LCD"
    };
    obj = modes[(int)mode];
}

void from_json(const json& obj, FontMode& mode) {
    static std::unordered_map<std::string, FontMode> modes{
        { "Normal", FontMode::Normal },
        { "LCD", FontMode::LCD }
    };
    mode = modes[obj.get<std::string>()];
}

void to_json(json& obj, const FontSettings& settings) {
    obj["atlasSize"] = settings.atlasSize;
    obj["fontMode"] = settings.fontMode;
    obj["spacesPerTab"] = settings.spacesPerTab;
    obj["padding"] = settings.padding;
    obj["margin"] = settings.margin;
}

void from_json(const json& obj, FontSettings& settings) {
    settings.atlasSize = obj.value("atlasSize", settings.atlasSize);
    settings.fontMode = obj.value("fontMode", settings.fontMode);
    settings.spacesPerTab = obj.value("spacesPerTab", settings.spacesPerTab);
    settings.padding = obj.value("padding", settings.padding);
    settings.margin = obj.value("margin", settings.margin);
}

} // data
} // mw