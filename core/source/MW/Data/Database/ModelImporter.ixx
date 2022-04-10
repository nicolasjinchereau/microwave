/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Data.Database.ModelImporter;
import Microwave.Data.Database.AssetDatabase;
import Microwave.Data.Database.AssetImporter;
import Microwave.Data.Library.AssetSettings;
import Microwave.Graphics.Material;
import Microwave.Graphics.Model;
import Microwave.IO.File;
import Microwave.IO.Stream;
import Microwave.SceneGraph.Node;
import Microwave.System.Json;
import Microwave.System.Path;
import Microwave.System.Pointers;
import Microwave.System.UUID;
import Microwave.Utilities.Util;
import <filesystem>;
import <fstream>;
import <span>;
import <string>;
import <unordered_map>;
import <unordered_set>;

export namespace mw {
inline namespace data {

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
