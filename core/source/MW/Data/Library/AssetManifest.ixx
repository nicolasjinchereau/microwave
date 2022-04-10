/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module;
#include <MW/System/Internal/Platform.h>

export module Microwave.Data.Library.AssetManifest;
import Microwave.Data.Library.AssetType;
import Microwave.Data.Library.AssetSettings;
import Microwave.IO.File;
import Microwave.System.Json;
import Microwave.System.Path;
import Microwave.System.UUID;
import <unordered_map>;
import <vector>;

export namespace mw {
inline namespace data {

struct AssetArtifact
{
    UUID uuid;
    path sourcePath;
    AssetType assetType{};
    json settings;
};

class AssetManifest
{
public:
    std::vector<AssetArtifact> artifacts;
};

void to_json(json& obj, const AssetArtifact& art) {
    obj["uuid"] = art.uuid;
    obj["assetType"] = art.assetType;
    obj["sourcePath"] = art.sourcePath;
    obj["settings"] = art.settings;
}

void from_json(const json& obj, AssetArtifact& art) {
    art.uuid = obj.value("uuid", art.uuid);
    art.assetType = obj.value("assetType", art.assetType);
    art.sourcePath = obj.value("sourcePath", art.sourcePath);
    art.settings = obj.value("settings", json(nullptr));
}

void to_json(json& obj, const AssetManifest& manifest) {
    obj["artifacts"] = manifest.artifacts;
}

void from_json(const json& obj, AssetManifest& manifest) {
    manifest.artifacts = obj.value("artifacts", manifest.artifacts);
}

}
}
