/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Model;
import Microwave.Graphics.AnimationTrack;
import Microwave.Graphics.Color;
import Microwave.Graphics.GraphicsTypes;
import Microwave.Graphics.Mesh;
import Microwave.Graphics.RenderQueue;
import Microwave.IO.Stream;
import Microwave.Math;
import Microwave.System.Json;
import Microwave.System.Path;
import Microwave.System.Pointers;
import <cstdint>;
import <span>;
import <string>;
import <vector>;
import <unordered_map>;

export namespace mw {
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
    gptr<ModelMaterial> material;
};

class ModelMesh
{
public:
    std::string name;

    std::vector<Vec3> vertices;
    std::vector<Vec3> normals;
    std::vector<Vec2> texcoords;
    gvector<ModelMeshElement> elements;

    SkinType skinType = SkinType::None;
    std::vector<Bone> bones;
    std::vector<IVec4> boneIndices; // 4 indices per vertex
    std::vector<Vec4> boneWeights; // 4 weights per vertex
};

enum class ModelColliderType
{
    None,
    Sphere,  // radius
    Box,     // length, width, height
    Capsule, // radius, height
    Convex   // mesh
};

enum class ModelRigidBodyType
{
    None,
    Dynamic,
    Kinematic,
    Static
};

struct ModelCollider
{
    ModelColliderType type = ModelColliderType::None;
    ModelRigidBodyType bodyType = ModelRigidBodyType::None;
    std::string name;
    std::string pivotID;
    float radius = {};
    float length = {}; // X (3dsmax X)
    float height = {}; // Y (3dsmax Z)
    float width = {};  // Z (3dsmax Y)
    float friction = 0.5f;
    gptr<ModelMesh> mesh;
};

struct ModelNode
{
    uint32_t index = 0;
    std::string name;
    bool active = true;
    Vec3 localPosition;
    Quat localRotation;
    Vec3 localScale = Vec3::One();
    gptr<ModelMesh> mesh;
    gptr<ModelCollider> collider;
    ModelNode* parent = nullptr;
    gvector<gptr<ModelNode>> children;

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
    gmap<std::string, gptr<AnimationTrack>> tracks;
};

class Model
{
    constexpr static const char* InternalFilename = "model.json";
public:
    int version = 1;
    float animationFPS = 0;
    gptr<ModelNode> rootNode;
    gvector<gptr<ModelAnimationClip>> clips;

    // filled on load
    gvector<gptr<ModelNode>> nodes;

    static gptr<Model> Load(const gptr<Stream>& stream);
    static void Save(const gptr<Model>& model, const path& ouptut);
};

void to_json(json& obj, const ModelMaterial& material);
void from_json(const json& obj, ModelMaterial& material);

void to_json(json& obj, const ModelMeshElement& element);
void from_json(const json& obj, ModelMeshElement& element);

void to_json(json& obj, const ModelMesh& mesh);
void from_json(const json& obj, ModelMesh& mesh);

void to_json(json& obj, const ModelColliderType& type);
void from_json(const json& obj, ModelColliderType& type);

void to_json(json& obj, const ModelRigidBodyType& type);
void from_json(const json& obj, ModelRigidBodyType& type);

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
