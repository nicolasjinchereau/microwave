/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/System/Json.h>
#include <MW/Graphics/Mesh.h>
#include <MW/System/App.h>
#include <algorithm>
#include <unordered_map>

namespace mw {
inline namespace gfx {

void Mesh::ToJson(json& obj) const
{
    Object::ToJson(obj);

    obj["vertices"] = vertices;
    obj["normals"] = normals;
    obj["texcoords"] = texcoords;
    obj["elements"] = elements;
    obj["skinType"] = skinType;
    obj["bones"] = bones;
    obj["boneIndices"] = boneIndices;
    obj["boneWeights"] = boneWeights;
    obj["bsphere"] = bsphere;
    obj["bbox"] = bbox;
}

void Mesh::FromJson(const json& obj, ObjectLinker* linker)
{
    Object::FromJson(obj, linker);
    
    vertices = obj.value("vertices", vertices);
    normals = obj.value("normals", normals);
    texcoords = obj.value("texcoords", texcoords);
    elements = obj.value("elements", elements);
    skinType = obj.value("skinType", skinType);
    bones = obj.value("bones", bones);
    boneIndices = obj.value("boneIndices", boneIndices);
    boneWeights = obj.value("boneWeights", boneWeights);
    bsphere = obj.value("bsphere", bsphere);
    bbox = obj.value("bbox", bbox);

    UpdateBuffers();
}

void Mesh::RecalcBounds()
{
    if (!vertices.empty())
    {
        Vec3 vmin = vertices[0];
        Vec3 vmax = vertices[0];
        size_t nVerts = vertices.size();

        for (size_t v = 1; v < nVerts; v++)
        {
            vmin.x = std::min(vmin.x, vertices[v].x);
            vmin.y = std::min(vmin.y, vertices[v].y);
            vmin.z = std::min(vmin.z, vertices[v].z);
            vmax.x = std::max(vmax.x, vertices[v].x);
            vmax.y = std::max(vmax.y, vertices[v].y);
            vmax.z = std::max(vmax.z, vertices[v].z);
        }

        Vec3 extents = (vmax - vmin) * 0.5f;
        Vec3 center = vmin + extents;
        float radius = 0.0f;

        for (size_t v = 0; v < nVerts; v++)
            radius = std::max(radius, (vertices[v] - center).LengthSq());

        if (radius > 0.0f)
            radius = sqrt(radius);

        bsphere = Sphere(center, radius);
        bbox = AABox(center, extents);
    }
}

void Mesh::UpdateBuffers()
{
    auto graphics = App::Get()->GetGraphics();
    bool dynamicBuffers = skinType != SkinType::None;
    
    if (!vertices.empty())
    {
        auto pVertices = (std::byte*)vertices.data();
        auto szVertices = vertices.size() * sizeof(Vec3);

        vertexBuffer = graphics->CreateBuffer(
            szVertices,
            DrawBufferType::Vertex,
            dynamicBuffers,
            std::span<std::byte>(pVertices, szVertices)
        );
    }
    else
    {
        vertexBuffer = nullptr;
    }

    if (!normals.empty())
    {
        auto pNormals = (std::byte*)normals.data();
        auto szNormals = normals.size() * sizeof(Vec3);
        normalBuffer = graphics->CreateBuffer(
            szNormals,
            DrawBufferType::Vertex,
            dynamicBuffers,
            std::span<std::byte>(pNormals, szNormals)
        );
    }
    else
    {
        normalBuffer = nullptr;
    }

    if (!texcoords.empty())
    {
        auto pUVs = (std::byte*)texcoords.data();
        auto szUVs = texcoords.size() * sizeof(Vec2);
        texcoordBuffer = graphics->CreateBuffer(
            szUVs,
            DrawBufferType::Vertex,
            false,
            std::span<std::byte>(pUVs, szUVs)
        );
    }
    else
    {
        texcoordBuffer = nullptr;
    }

    if (!boneIndices.empty())
    {
        auto pBoneIndices = (std::byte*)boneIndices.data();
        auto szBoneIndices = boneIndices.size() * sizeof(IVec4);
        boneIndexBuffer = graphics->CreateBuffer(
            szBoneIndices,
            DrawBufferType::Vertex,
            false,
            std::span<std::byte>(pBoneIndices, szBoneIndices)
        );
    }
    else
    {
        boneIndexBuffer = nullptr;
    }

    if (!boneWeights.empty())
    {
        auto pBoneWeights = (std::byte*)boneWeights.data();
        auto szBoneWeights = boneWeights.size() * sizeof(Vec4);
        boneWeightBuffer = graphics->CreateBuffer(
            szBoneWeights,
            DrawBufferType::Vertex,
            false,
            std::span<std::byte>(pBoneWeights, szBoneWeights)
        );
    }
    else
    {
        boneWeightBuffer = nullptr;
    }

    for (auto& elem : elements)
    {
        if (!elem.indices.empty())
        {
            auto pIndices = (std::byte*)elem.indices.data();
            auto szIndices = elem.indices.size() * sizeof(int);

            elem.indexBuffer = graphics->CreateBuffer(
                szIndices,
                DrawBufferType::Index,
                false,
                std::span<std::byte>(pIndices, szIndices)
            );
        }
        else
        {
            elem.indexBuffer = nullptr;
        }
    }
}

void to_json(json& obj, const BoneLinkMode& linkMode)
{
    std::array<const char*, 3> modes{
        "Normalize",
        "Additive",
        "TotalOne"
    };

    obj = modes[(int)linkMode];
}

void from_json(const json& obj, BoneLinkMode& linkMode)
{
    auto val = obj.get<std::string>("Normalize");
    if (val == "Normalize")
        linkMode = BoneLinkMode::Normalize;
    else if (val == "Additive")
        linkMode = BoneLinkMode::Additive;
    else if (val == "TotalOne")
        linkMode = BoneLinkMode::TotalOne;
    else
        throw std::runtime_error("invalid bone link mode: " + val);
}

void to_json(json& obj, const Bone& bone)
{
    obj["linkNodePath"] = bone.linkNodePath;
    obj["linkMode"] = bone.linkMode;
    obj["boneBindMatrix"] = bone.boneBindMatrix;
    obj["meshBindMatrix"] = bone.meshBindMatrix;
    obj["invBoneBindMatrix"] = bone.invBoneBindMatrix;
}

void from_json(const json& obj, Bone& bone)
{
    bone.linkNodePath = obj.value("linkNodePath", bone.linkNodePath);
    bone.linkMode = obj.value("linkMode", bone.linkMode);
    bone.boneBindMatrix = obj.value("boneBindMatrix", bone.boneBindMatrix);
    bone.meshBindMatrix = obj.value("meshBindMatrix", bone.meshBindMatrix);
    bone.invBoneBindMatrix = obj.value("invBoneBindMatrix", bone.invBoneBindMatrix);
}

void to_json(json& obj, const MeshElement& elem)
{
    obj["drawMode"] = elem.drawMode;
    obj["indices"] = elem.indices;
}

void from_json(const json& obj, MeshElement& elem)
{
    elem.drawMode = obj.value("drawMode", elem.drawMode);
    elem.indices = obj.value("indices", elem.indices);
}

void to_json(json& obj, const SkinType& skinType)
{
    std::array<const char*, 5> modes{
        "None",
        "Rigid",
        "Linear",
        "DualQuaternion",
        "Blend"
    };

    obj = modes[(int)skinType];
}

void from_json(const json& obj, SkinType& skinType)
{
    auto val = obj.get<std::string>("None");
    if (val == "None")
        skinType = SkinType::None;
    else if (val == "Rigid")
        skinType = SkinType::Rigid;
    else if (val == "Linear")
        skinType = SkinType::Linear;
    else if (val == "DualQuaternion")
        skinType = SkinType::DualQuaternion;
    else if (val == "Blend")
        skinType = SkinType::Blend;
    else
        throw std::runtime_error("invalid bone link mode: " + val);
}

void to_json(json& obj, const Mesh& mesh) {
    mesh.ToJson(obj);
}

void from_json(const json& obj, Mesh& mesh) {
    mesh.FromJson(obj, nullptr);
}

}
}
