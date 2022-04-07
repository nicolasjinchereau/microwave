/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Graphics/Color.h>
#include <MW/Graphics/DrawBuffer.h>
#include <MW/Graphics/Material.h>
#include <MW/Math/Math.h>
#include <MW/System/Object.h>
#include <MW/System/path.h>
#include <MW/System/Pointers.h>
#include <span>
#include <vector>

namespace mw {
inline namespace gfx {

enum class BoneLinkMode : int
{
    Normalize,
    Additive,
    TotalOne
};

enum class SkinType : int
{
    None,
    Rigid,
    Linear,
    DualQuaternion, // not supported
    Blend           // not supported
};

class Bone
{
public:
    path linkNodePath;
    BoneLinkMode linkMode = BoneLinkMode::Normalize;
    Mat4 boneBindMatrix;
    Mat4 meshBindMatrix;
    Mat4 invBoneBindMatrix;
};

struct MeshElement
{
    DrawMode drawMode = DrawMode::Triangles;
    std::vector<int> indices;
    sptr<DrawBuffer> indexBuffer;
};

class Mesh : public Object
{
    REGISTER_OBJECT(Mesh);
public:
    std::vector<Vec3> vertices;
    std::vector<Vec3> normals;
    std::vector<Vec2> texcoords;
    std::vector<MeshElement> elements;

    SkinType skinType;
    std::vector<Bone> bones;
    std::vector<IVec4> boneIndices; // 4 indices per vertex
    std::vector<Vec4> boneWeights; // 4 weights per vertex

    sptr<DrawBuffer> vertexBuffer;
    sptr<DrawBuffer> normalBuffer;
    sptr<DrawBuffer> texcoordBuffer;
    sptr<DrawBuffer> boneIndexBuffer;
    sptr<DrawBuffer> boneWeightBuffer;

    Sphere bsphere;
    AABox bbox;

    virtual void ToJson(json& obj) const override;
    virtual void FromJson(const json& obj, ObjectLinker* linker) override;

    void RecalcBounds();
    void UpdateBuffers();
};


void to_json(json& obj, const BoneLinkMode& linkMode);
void from_json(const json& obj, BoneLinkMode& linkMode);

void to_json(json& obj, const Bone& bone);
void from_json(const json& obj, Bone& bone);

void to_json(json& obj, const MeshElement& elem);
void from_json(const json& obj, MeshElement& elem);

void to_json(json& obj, const SkinType& skinType);
void from_json(const json& obj, SkinType& skinType);

void to_json(json& obj, const Mesh& mesh);
void from_json(const json& obj, Mesh& mesh);

}
}
