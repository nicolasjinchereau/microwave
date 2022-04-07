/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/System/Json.h>
#include <MW/Graphics/AnimationTrack.h>
#include <MW/Graphics/Color.h>
#include <MW/Graphics/Mesh.h>
#include <MW/Graphics/RenderQueue.h>
#include <MW/Math/Math.h>
#include <MW/System/Pointers.h>
#include <MW/IO/Stream.h>
#include <span>
#include <vector>

namespace mw {
inline namespace io {
class FileStream;
}
}

namespace mw {
inline namespace gfx {

struct ModelMaterial
{
    std::string  name;

    path   uDiffuseTex;
    Vec4   uDiffuseTex_ST = Vec4::Zero();
    float  uDiffuseFactor = 1.0f;
    Color  uDiffuseColor = Color::White();

    path   uSpecularTex;
    Vec4   uSpecularTex_ST = Vec4::Zero();
    float  uSpecularFactor = 0.0f;
    Color  uSpecularColor = Color::White();
    float  uShininess = 0.0f;

    path   uNormalTex;
    Vec4   uNormalTex_ST = Vec4::Zero();
    float  uNormalFactor = 1.0f;

    BlendOperation colorBlendOperation = BlendOperation::Add;
    BlendOperation alphaBlendOperation = BlendOperation::Add;
    BlendFactor sourceColorBlendFactor = BlendFactor::One;
    BlendFactor sourceAlphaBlendFactor = BlendFactor::One;
    BlendFactor destColorBlendFactor = BlendFactor::Zero;
    BlendFactor destAlphaBlendFactor = BlendFactor::Zero;
    CullMode cullMode = CullMode::Back;
    DepthTest depthTest = DepthTest::LessOrEqual;
    bool depthWriteEnabled = true;
    uint32_t renderQueue = RenderQueue::Opaque;
};

struct ModelMeshElement
{
    DrawMode drawMode = DrawMode::Triangles;
    std::vector<int> indices;
    sptr<ModelMaterial> material;
};

class ModelMesh
{
public:
    std::string name;

    std::vector<Vec3> vertices;
    std::vector<Vec3> normals;
    std::vector<Vec2> texcoords;
    std::vector<ModelMeshElement> elements;

    SkinType skinType; 
    std::vector<Bone> bones;
    std::vector<IVec4> boneIndices; // 4 indices per vertex
    std::vector<Vec4> boneWeights; // 4 weights per vertex
};

enum class ModelColliderType
{
    Sphere,  // radius
    Box,     // length, width, height
    Capsule, // radius, height
    Convex   // mesh
};

enum class ModelRigidBodyType
{
    Dynamic,
    Kinematic,
    Static
};

struct ModelCollider
{
    ModelColliderType type;
    ModelRigidBodyType bodyType;
    std::string name;
    std::string pivotID;
    float radius = {};
    float length = {}; // X (3dsmax X)
    float height = {}; // Y (3dsmax Z)
    float width = {};  // Z (3dsmax Y)
    float friction = 0.5f;
    sptr<ModelMesh> mesh;
};

struct ModelNode
{
    uint32_t index = 0;
    std::string name;
    bool active = true;
    Vec3 localPosition;
    Quat localRotation;
    Vec3 localScale = Vec3::One();
    sptr<ModelMesh> mesh;
    sptr<ModelCollider> collider;
    ModelNode* parent = nullptr;
    std::vector<sptr<ModelNode>> children;

    Vec3 position() const;
    Quat rotation() const;
    Vec3 scale() const;
    path GetFullPath() const;
    path GetRelativePath(ModelNode* toParent) const;
};

enum class ModelAnimationWrapMode
{
    Once,
    Sustain,
    Loop
};

struct ModelAnimationClip
{
    std::string name;
    ModelAnimationWrapMode wrapMode = ModelAnimationWrapMode::Loop;
    std::unordered_map<std::string, sptr<AnimationTrack>> tracks;
};

class Model
{
    constexpr static const char* InternalFilename = "model.json";
public:
    int version = 1;
    float animationFPS = 0;
    sptr<ModelNode> rootNode;
    std::vector<sptr<ModelAnimationClip>> clips;

    // filled on load
    std::vector<sptr<ModelNode>> nodes;

    static sptr<Model> Load(const sptr<Stream>& stream);
    static void Save(const sptr<Model>& model, const path& ouptut);
};

void to_json(json& obj, const ModelMaterial& material);
void from_json(const json& obj, ModelMaterial& material);

void to_json(json& obj, const ModelMeshElement& element);
void from_json(const json& obj, ModelMeshElement& element);

void to_json(json& obj, const ModelMesh& mesh);
void from_json(const json& obj, ModelMesh& mesh);

void to_json(json& obj, const ModelColliderType& type);
void from_json(const json& obj, ModelColliderType& type);

void to_json(json& obj, const ModelCollider& collider);
void from_json(const json& obj, ModelCollider& collider);

void to_json(json& obj, const ModelNode& node);
void from_json(const json& obj, ModelNode& node);

void to_json(json& obj, const ModelAnimationWrapMode& wrapMode);
void from_json(const json& obj, ModelAnimationWrapMode& wrapMode);

void to_json(json& obj, const ModelAnimationClip& clip);
void from_json(const json& obj, ModelAnimationClip& clip);

void to_json(json& obj, const Model& model);
void from_json(const json& obj, Model& model);

}
}
