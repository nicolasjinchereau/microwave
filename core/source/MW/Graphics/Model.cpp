/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/System/Json.h>
#include <MW/Graphics/Model.h>
#include <MW/IO/FileStream.h>
#include <MW/Utilities/Base64.h>
#include <zip.h>

namespace mw {
inline namespace gfx {

Vec3 ModelNode::position() const
{
    if (parent) {
        Vec3 pos = localPosition.Multiply(parent->scale());
        return parent->position() + pos * parent->rotation();
    }

    return localPosition;
}

Quat ModelNode::rotation() const
{
    if (parent) {
        return parent->rotation() * localRotation;
    }

    return localRotation;
}

Vec3 ModelNode::scale() const
{
    if (parent) {
        return localScale.Multiply(parent->scale());
    }

    return localScale;
}

namespace {
    void getFullPathHelper(
        const ModelNode* node,
        std::string& fullPath,
        int reserve = 0)
    {
        ModelNode* parent = node->parent;
        if (parent)
        {
            int res = reserve;
            if (res) res += 1;
            res += node->name.size();

            getFullPathHelper(parent, fullPath, res);

            if (!fullPath.empty())
                fullPath += "/";

            fullPath += node->name;
        }
        else
        {
            fullPath.reserve(reserve);
        }
    }

    void getRelativePathHelper(
        const ModelNode* node,
        const ModelNode* toParent,
        std::string& fullPath,
        int reserve = 0)
    {
        if (node != toParent)
        {
            int res = reserve;
            if (res) res += 1;
            res += node->name.size();

            getFullPathHelper(node->parent, fullPath, res);

            if (!fullPath.empty())
                fullPath += "/";

            fullPath += node->name;
        }
        else
        {
            fullPath.reserve(reserve);
        }
    }
}

path ModelNode::GetFullPath() const
{
    std::string fullPath;
    getFullPathHelper(this, fullPath);
    return path(std::move(fullPath));
}

path ModelNode::GetRelativePath(ModelNode* toParent) const
{
    std::string relativePath;
    getRelativePathHelper(this, toParent, relativePath);
    return path(std::move(relativePath));
}

sptr<Model> Model::Load(const sptr<Stream>& stream)
{
    auto model = spnew<Model>();

    zip_t* zip = nullptr;
    zip_source* source = nullptr;

    zip_error_t zerror;
    zip_error_init(&zerror);

    try
    {
        std::vector<std::byte> fileData;
        fileData.resize(stream->GetLength());
        stream->Read(fileData);

        zip_source* source = zip_source_buffer_create(
            fileData.data(), fileData.size(), 0, &zerror);

        if (!source)
            throw std::exception();

        zip = zip_open_from_source(source, 0, &zerror);
        if (!zip)
            throw std::runtime_error("failed to open zip file");

        struct zip_stat st;
        zip_stat_init(&st);
        int error = zip_stat(zip, InternalFilename, 0, &st);
        if (error < 0)
            throw std::exception();

        std::string text;
        text.resize((size_t)st.size);

        zip_file* file = zip_fopen(zip, InternalFilename, 0);
        if (!file)
            throw std::exception();

        size_t read = (size_t)zip_fread(file, text.data(), st.size);
        if (read != st.size)
            throw std::exception();

        error = zip_fclose(file);
        if (error < 0)
            throw std::exception();

        zip_discard(zip);
        zip = nullptr;
        source = nullptr;

        json obj = json::parse(text);

        *model = obj;
    }
    catch (const std::exception& ex)
    {
        std::string error = zip ? zip_strerror(zip) : ex.what();

        if (source) {
            zip_source_free(source);
            source = nullptr;
        }

        if (zip) {
            zip_discard(zip);
            zip = nullptr;
        }

        throw std::runtime_error(error);
    }

    return model;
}

void Model::Save(const sptr<Model>& model, const path& ouptut)
{
    json obj = *model;
    std::string text = obj.dump();

    zip_t* zip = nullptr;

    try
    {
        int error = 0;

        zip = zip_open(ouptut.c_str(), ZIP_CREATE | ZIP_TRUNCATE, &error);
        if (!zip)
            throw std::runtime_error("failed to create zip file");

        zip_source_t* source = zip_source_buffer(zip, text.data(), text.size(), 0);
        if (!source)
            throw std::exception();

        auto res = zip_file_add(zip, InternalFilename, source, ZIP_FL_ENC_UTF_8);
        if (res < 0)
            throw std::exception();

        auto index = zip_name_locate(zip, InternalFilename, 0);
        if (index < 0)
            throw std::exception();

        // 0 - 9
        float compression = 1.0f;
        int compressionLevel = (int)(compression * 9 + 0.5f);
        compressionLevel = std::min(std::max(compressionLevel, 0), 9);

        int result = zip_set_file_compression(zip, index, ZIP_CM_XZ, compressionLevel);
        if (result < 0)
            throw std::runtime_error("mgz file compression failed");

        result = zip_close(zip);
        if (result < 0)
            throw std::exception();
    }
    catch (const std::exception& ex)
    {
        std::string error = zip ? zip_strerror(zip) : ex.what();

        if (zip) {
            zip_discard(zip);
            zip = nullptr;
        }

        throw std::runtime_error(error);
    }
}

void to_json(json& obj, const ModelMaterial& mat)
{
    obj["name"] = mat.name;

    obj["uDiffuseTex"] = mat.uDiffuseTex;
    obj["uDiffuseTex_ST"] = mat.uDiffuseTex_ST;
    obj["uDiffuseFactor"] = mat.uDiffuseFactor;
    obj["uDiffuseColor"] = mat.uDiffuseColor;

    obj["uSpecularTex"] = mat.uSpecularTex;
    obj["uSpecularTex_ST"] = mat.uSpecularTex_ST;
    obj["uSpecularFactor"] = mat.uSpecularFactor;
    obj["uSpecularColor"] = mat.uSpecularColor;
    obj["uShininess"] = mat.uShininess;

    obj["uNormalTex"] = mat.uNormalTex;
    obj["uNormalTex_ST"] = mat.uNormalTex_ST;
    obj["uNormalFactor"] = mat.uNormalFactor;

    obj["colorBlendOperation"] = mat.colorBlendOperation;
    obj["alphaBlendOperation"] = mat.alphaBlendOperation;
    obj["sourceColorBlendFactor"] = mat.sourceColorBlendFactor;
    obj["sourceAlphaBlendFactor"] = mat.sourceAlphaBlendFactor;
    obj["destColorBlendFactor"] = mat.destColorBlendFactor;
    obj["destAlphaBlendFactor"] = mat.destAlphaBlendFactor;
    obj["cullMode"] = mat.cullMode;
    obj["depthTest"] = mat.depthTest;
    obj["depthWriteEnabled"] = mat.depthWriteEnabled;
    obj["renderQueue"] = mat.renderQueue;
}

void from_json(const json& obj, ModelMaterial& mat)
{
    mat.name = obj.value("name", mat.name);

    mat.uDiffuseTex = obj.value("uDiffuseTex", mat.uDiffuseTex);
    mat.uDiffuseTex_ST = obj.value("uDiffuseTex_ST", mat.uDiffuseTex_ST);
    mat.uDiffuseFactor = obj.value("uDiffuseFactor", mat.uDiffuseFactor);
    mat.uDiffuseColor = obj.value("uDiffuseColor", mat.uDiffuseColor);

    mat.uSpecularTex = obj.value("uSpecularTex", mat.uSpecularTex);
    mat.uSpecularTex_ST = obj.value("uSpecularTex_ST", mat.uSpecularTex_ST);
    mat.uSpecularFactor = obj.value("uSpecularFactor", mat.uSpecularFactor);
    mat.uSpecularColor = obj.value("uSpecularColor", mat.uSpecularColor);
    mat.uShininess = obj.value("uShininess", mat.uShininess);

    mat.uNormalTex = obj.value("uNormalTex", mat.uNormalTex);
    mat.uNormalTex_ST = obj.value("uNormalTex_ST", mat.uNormalTex_ST);
    mat.uNormalFactor = obj.value("uNormalFactor", mat.uNormalFactor);

    mat.colorBlendOperation = obj["colorBlendOperation"];
    mat.alphaBlendOperation = obj["alphaBlendOperation"];
    mat.sourceColorBlendFactor = obj["sourceColorBlendFactor"];
    mat.sourceAlphaBlendFactor = obj["sourceAlphaBlendFactor"];
    mat.destColorBlendFactor = obj["destColorBlendFactor"];
    mat.destAlphaBlendFactor = obj["destAlphaBlendFactor"];
    mat.cullMode = obj["cullMode"];
    mat.depthTest = obj["depthTest"];
    mat.depthWriteEnabled = obj["depthWriteEnabled"];
    mat.renderQueue = obj["renderQueue"];
}

void to_json(json& obj, const ModelMeshElement& elem)
{
    obj["indices"] = elem.indices;
    obj["material"] = elem.material;
}

void from_json(const json& obj, ModelMeshElement& elem)
{
    elem.indices = obj.value("indices", elem.indices);
    elem.material = obj.value("material", elem.material);
}

void to_json(json& obj, const ModelMesh& mesh)
{
    obj["vertices"] = mesh.vertices;
    obj["normals"] = mesh.normals;
    obj["texcoords"] = mesh.texcoords;
    obj["elements"] = mesh.elements;
    obj["skinType"] = mesh.skinType;
    obj["bones"] = mesh.bones;
    obj["boneIndices"] = mesh.boneIndices;
    obj["boneWeights"] = mesh.boneWeights;
}

void from_json(const json& obj, ModelMesh& mesh)
{
    mesh.vertices = obj.value("vertices", mesh.vertices);
    mesh.normals = obj.value("normals", mesh.normals);
    mesh.texcoords = obj.value("texcoords", mesh.texcoords);
    mesh.elements = obj.value("elements", mesh.elements);
    mesh.skinType = obj.value("skinType", mesh.skinType);
    mesh.bones = obj.value("bones", mesh.bones);
    mesh.boneIndices = obj.value("boneIndices", mesh.boneIndices);
    mesh.boneWeights = obj.value("boneWeights", mesh.boneWeights);
}

void to_json(json& obj, const ModelColliderType& type)
{
    static const char* typeNames[]{ "Sphere", "Box", "Capsule", "Convex" };
    obj = typeNames[(int)type];
}

void from_json(const json& obj, ModelColliderType& type)
{
    static std::unordered_map<std::string, ModelColliderType> modes {
        { "Sphere", ModelColliderType::Sphere },
        { "Box", ModelColliderType::Box },
        { "Capsule", ModelColliderType::Capsule },
        { "Convex", ModelColliderType::Convex }
    };
    type = modes[obj.get<std::string>("Invalid")];
}

void to_json(json& obj, const ModelCollider& collider)
{
    obj["type"] = collider.type;
    obj["name"] = collider.name;
    obj["pivotID"] = collider.pivotID;
    obj["radius"] = collider.radius;
    obj["length"] = collider.length;
    obj["height"] = collider.height;
    obj["width"] = collider.width;
    obj["friction"] = collider.friction;
    obj["mesh"] = collider.mesh;
}

void from_json(const json& obj, ModelCollider& collider)
{
    collider.type = obj.value("type", collider.type);
    collider.name = obj.value("name", collider.name);
    collider.pivotID = obj.value("pivotID", collider.pivotID);
    collider.radius = obj.value("radius", collider.radius);
    collider.length = obj.value("length", collider.length);
    collider.height = obj.value("height", collider.height);
    collider.width = obj.value("width", collider.width);
    collider.friction = obj.value("friction", collider.friction);
    collider.mesh = obj.value("mesh", collider.mesh);
}

void to_json(json& obj, const ModelNode& node)
{
    obj["index"] = node.index;
    obj["name"] = node.name;
    obj["active"] = node.active;

    obj["localPosition"] = node.localPosition;
    obj["localRotation"] = node.localRotation;
    obj["localScale"] = node.localScale;

    obj["mesh"] = node.mesh;
    obj["collider"] = node.collider;

    auto& children = obj["children"];
    children = std::vector<json>();

    for (auto c : node.children) {
        json cobj;
        to_json(cobj, *c);
        children.push_back(cobj);
    }
}

void from_json(const json& obj, ModelNode& node)
{
    node.index = obj.value("index", node.index);
    node.name = obj.value("name", node.name);
    node.active = obj.value("active", node.active);

    node.localPosition = obj.value("localPosition", node.localPosition);
    node.localRotation = obj.value("localRotation", node.localRotation);
    node.localScale = obj.value("localScale", node.localScale);
    
    node.mesh = obj.value("mesh", node.mesh);
    node.collider = obj.value("collider", node.collider);

    if (auto it = obj.find("children"); it != obj.end())
    {
        for (auto& cobj : *it)
        {
            auto c = spnew<ModelNode>();
            from_json(cobj, *c);
            c->parent = &node;
            node.children.push_back(c);
        }
    }
}

void to_json(json& obj, const ModelAnimationWrapMode& wrapMode)
{
    static const char* modeNames[]{ "Once", "Sustain", "Loop", };
    obj = modeNames[(int)wrapMode];
}

void from_json(const json& obj, ModelAnimationWrapMode& wrapMode)
{
    static std::unordered_map<std::string, ModelAnimationWrapMode> modes{
        { "Once", ModelAnimationWrapMode::Once },
        { "Sustain", ModelAnimationWrapMode::Sustain },
        { "Loop", ModelAnimationWrapMode::Loop }
    };
    wrapMode = modes[obj.get<std::string>("Once")];
}

void to_json(json& obj, const ModelAnimationClip& clip)
{
    obj["name"] = clip.name;
    obj["wrapMode"] = clip.wrapMode;

    json trackObjs = json::object();
    for (auto& [path, track] : clip.tracks)
        trackObjs[path] = *track;

    obj["tracks"] = std::move(trackObjs);
}

void from_json(const json& obj, ModelAnimationClip& clip)
{
    clip.name = obj["name"];
    clip.wrapMode = obj["wrapMode"];

    const json& trackObjs = obj["tracks"];
    for (auto& [key, trackObj] : trackObjs.items())
    {
        auto track = spnew<AnimationTrack>();
        from_json(trackObj, *track);
        clip.tracks[key] = track;
    }
}


namespace {

size_t CountModelNodes(const sptr<ModelNode>& node)
{
    size_t count = 1;

    for (auto& child : node->children)
        count += CountModelNodes(child);

    return count;
};

void StoreModelNodes(
    const sptr<ModelNode>& node,
    std::vector<sptr<ModelNode>>& nodes)
{
    nodes[node->index] = node;

    for (auto& child : node->children) {
        StoreModelNodes(child, nodes);
    }
};

}

void to_json(json& obj, const Model& model)
{
    obj["version"] = model.version;
    obj["animationFPS"] = model.animationFPS;
    obj["rootNode"] = *model.rootNode;

    auto clipsObj = json::array();

    for (auto& clip : model.clips)
        clipsObj.push_back(json(*clip));

    obj["clips"] = clipsObj;
}

void from_json(const json& obj, Model& model)
{
    model.version = obj.value("version", model.version);
    model.animationFPS = obj.value("animationFPS", model.animationFPS);

    if (auto it = obj.find("rootNode"); it != obj.end())
    {
        model.rootNode = spnew<ModelNode>();
        from_json(*it, *model.rootNode);
    }

    size_t nodeCount = CountModelNodes(model.rootNode);
    model.nodes.resize(nodeCount);
    StoreModelNodes(model.rootNode, model.nodes);

    const json& clipsObj = obj["clips"];
    for (auto& clipObj : clipsObj)
    {
        auto clip = spnew<ModelAnimationClip>();
        from_json(clipObj, *clip);
        model.clips.push_back(std::move(clip));
    }
}

}
}
