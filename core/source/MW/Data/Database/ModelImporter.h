/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Data/Database/AssetImporter.h>
#include <MW/Data/Library/AssetSettings.h>
#include <MW/System/Json.h>
#include <MW/Graphics/Model.h>
#include <MW/IO/File.h>
#include <MW/Scene/Node.h>
#include <MW/System/Platforms.h>
#include <MW/System/path.h>
#include <MW/System/UUID.h>
#include <MW/Utilities/Util.h>
#include <filesystem>
#include <fstream>
#include <unordered_set>

namespace mw {
inline namespace data {

class AssetDatabase;

struct MaterialInfo
{
    sptr<Material> material;
    AssetBinding shaderBinding;
    std::unordered_map<std::string, AssetBinding> textureBindings;
};

class ModelImporter : public AssetImporter
{
public:
    struct ParserState;

    virtual std::span<std::string> GetSupportedFileTypes() override;

    virtual void ImportFile(
        AssetMetadata& meta,
        const sptr<Stream>& stream,
        const path& dataDir) override;

    virtual bool Resolve(
        AssetDatabase& assetDatabase,
        AssetMetadata& meta,
        const path& sourceDir,
        const path& dataDir) override;

    void ParseModel(const sptr<Model>& model, ParserState& state);
    sptr<Node> ParseModelNode(const sptr<ModelNode>& modelNode, ParserState& state);
};

}
}
