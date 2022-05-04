export module Microwave.Data.Database.Metadata;
import Microwave.Data.Library.AssetType;
import Microwave.System.Json;
import Microwave.System.Path;
import Microwave.System.Pointers;
import Microwave.System.UUID;
import <cstdint>;
import <unordered_map>;
import <vector>;

export namespace mw {
inline namespace data {

struct ArtifactMetadata
{
    UUID uuid;
    path sourcePath;
    AssetType assetType = {};
};

struct AssetMetadata
{
    path sourcePath;
    json settings;
    std::vector<ArtifactMetadata> artifacts;
    bool dirty = false;
};

struct ArtifactRecord
{
    uint64_t lastModified = 0;
};

struct AssetRecord
{
    uint64_t sourceLastModified = 0;
    uint64_t metaLastModified = 0;
    std::unordered_map<UUID, ArtifactRecord> artifactRecords;
};

void to_json(json& obj, const ArtifactMetadata& art) {
    obj["uuid"] = art.uuid;
    obj["assetType"] = art.assetType;
    obj["sourcePath"] = art.sourcePath;
}

void from_json(const json& obj, ArtifactMetadata& art) {
    art.uuid = obj.value("uuid", art.uuid);
    art.assetType = obj.value("assetType", art.assetType);
    art.sourcePath = obj.value("sourcePath", art.sourcePath);
}

void to_json(json& obj, const AssetMetadata& meta) {
    obj["sourcePath"] = meta.sourcePath;
    obj["settings"] = meta.settings;
    obj["artifacts"] = meta.artifacts;
}

void from_json(const json& obj, AssetMetadata& meta)
{
    meta.sourcePath = obj.value("sourcePath", meta.sourcePath);
    meta.settings = obj.value("settings", json::object());
    meta.artifacts = obj.value("artifacts", meta.artifacts);
}

void to_json(json& obj, const ArtifactRecord& record) {
    obj["lastModified"] = record.lastModified;
}

void from_json(const json& obj, ArtifactRecord& record) {
    record.lastModified = obj.value("lastModified", record.lastModified);
}

void to_json(json& obj, const AssetRecord& record) {
    obj["sourceLastModified"] = record.sourceLastModified;
    obj["metaLastModified"] = record.metaLastModified;
    obj["artifactRecords"] = record.artifactRecords;
}

void from_json(const json& obj, AssetRecord& record) {
    record.sourceLastModified = obj.value("sourceLastModified", record.sourceLastModified);
    record.metaLastModified = obj.value("metaLastModified", record.metaLastModified);
    record.artifactRecords = obj.value("artifactRecords", record.artifactRecords);
}

} // data
} // mw
