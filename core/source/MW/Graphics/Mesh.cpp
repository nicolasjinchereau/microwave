/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Graphics.Mesh;
import Microwave.Graphics.GraphicsContext;
import Microwave.System.Exception;
import <algorithm>;
import <unordered_map>;

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
    auto graphics = GraphicsContext::GetCurrent();
    bool dynamicBuffers = skinType != SkinType::None;

    BufferUsage usage{};
    BufferCPUAccess cpuAccess{};

    if(skinType != SkinType::None) {
        usage = BufferUsage::Dynamic;
        cpuAccess = BufferCPUAccess::WriteOnly;
    }
    else {
        usage = BufferUsage::Static;
        cpuAccess = BufferCPUAccess::None;
    }

    if (!vertices.empty())
    {
        vertexBuffer = gpnew<Buffer>(
            BufferType::Vertex, usage, cpuAccess,
            std::as_writable_bytes(std::span(vertices)));
    }
    else
    {
        vertexBuffer = nullptr;
    }

    if (!normals.empty())
    {
        normalBuffer = gpnew<Buffer>(
            BufferType::Vertex, usage, cpuAccess,
            std::as_writable_bytes(std::span(normals)));
    }
    else
    {
        normalBuffer = nullptr;
    }

    if (!texcoords.empty())
    {
        texcoordBuffer = gpnew<Buffer>(
            BufferType::Vertex, BufferUsage::Static, BufferCPUAccess::None,
            std::as_writable_bytes(std::span(texcoords)));
    }
    else
    {
        texcoordBuffer = nullptr;
    }

    if (!boneIndices.empty())
    {
        boneIndexBuffer = gpnew<Buffer>(
            BufferType::Vertex, BufferUsage::Static, BufferCPUAccess::None,
            std::as_writable_bytes(std::span(boneIndices)));
    }
    else
    {
        boneIndexBuffer = nullptr;
    }

    if (!boneWeights.empty())
    {
        boneWeightBuffer = gpnew<Buffer>(
            BufferType::Vertex, BufferUsage::Static, BufferCPUAccess::None,
            std::as_writable_bytes(std::span(boneWeights)));
    }
    else
    {
        boneWeightBuffer = nullptr;
    }

    for (auto& elem : elements)
    {
        if (!elem.indices.empty())
        {
            elem.indexBuffer = gpnew<Buffer>(
                BufferType::Index, BufferUsage::Static, BufferCPUAccess::None,
                std::as_writable_bytes(std::span(elem.indices)));
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
        throw Exception({ "invalid bone link mode: ", val });
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
        throw Exception({ "invalid bone link mode: ", val });
}

void to_json(json& obj, const Mesh& mesh) {
    mesh.ToJson(obj);
}

void from_json(const json& obj, Mesh& mesh) {
    mesh.FromJson(obj, nullptr);
}

} // gfx
} // mw
