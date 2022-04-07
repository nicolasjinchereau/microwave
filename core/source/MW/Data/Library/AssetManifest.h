/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Data/Library/AssetType.h>
#include <MW/Data/Library/AssetSettings.h>
#include <MW/System/Json.h>
#include <MW/System/path.h>
#include <MW/System/Platforms.h>
#include <MW/System/UUID.h>
#include <MW/IO/File.h>
#include <unordered_map>

namespace mw {
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

inline void to_json(json& obj, const AssetArtifact& art) {
    obj["uuid"] = art.uuid;
    obj["assetType"] = art.assetType;
    obj["sourcePath"] = art.sourcePath;
    obj["settings"] = art.settings;
}

inline void from_json(const json& obj, AssetArtifact& art) {
    art.uuid = obj.value("uuid", art.uuid);
    art.assetType = obj.value("assetType", art.assetType);
    art.sourcePath = obj.value("sourcePath", art.sourcePath);
    art.settings = obj.value("settings", json(nullptr));
}

inline void to_json(json& obj, const AssetManifest& manifest) {
    obj["artifacts"] = manifest.artifacts;
}

inline void from_json(const json& obj, AssetManifest& manifest) {
    manifest.artifacts = obj.value("artifacts", manifest.artifacts);
}

}
}
