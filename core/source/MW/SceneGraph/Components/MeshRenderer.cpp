/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.SceneGraph.Components.MeshRenderer;
import Microwave.Graphics.Shader;
import Microwave.SceneGraph.Node;
import <cassert>;

namespace mw {
inline namespace scene {

AABox MeshRenderer::GetBounds() const
{
    AABox ret;

    if (mesh) {
        auto mtx = GetNode()->GetLocalToWorldMatrix();
        ret = mesh->bbox.Transform(mtx);
    }

    return ret;
}

void MeshRenderer::SystemLateUpdate()
{
    if (!mesh || mesh->skinType == SkinType::None)
        return;

    auto rootBone = this->rootBone.lock();
    if (!rootBone)
        return;

    size_t boneCount = mesh->bones.size();
    size_t vertCount = mesh->vertices.size();

    boneMatrices.resize(boneCount);
    blendedVerts.resize(vertCount);
    blendedNorms.resize(vertCount);

    auto node = GetNode();

    // calculate all bone matrices
    for (uint32_t i = 0; i < mesh->bones.size(); ++i)
    {
        auto linkNode = rootBone->GetChild(mesh->bones[i].linkNodePath);

        boneMatrices[i] =
            mesh->bones[i].meshBindMatrix *     // mesh bind matrix
            mesh->bones[i].invBoneBindMatrix *  // bone inverse bind matrix
            linkNode->GetLocalToWorldMatrix() * // bone current matrix
            node->GetWorldToLocalMatrix();  // mesh current inverse matrix
    }

    BoneLinkMode mode = mesh->bones[0].linkMode;

    // transform all vertices
    int nVerts = (int)mesh->vertices.size();

    uint32_t maxBoneCount = 4; // 1-4

    for (int i = 0; i < nVerts; ++i)
    {
        Mat4 deformation = (mode == BoneLinkMode::Additive) ? Mat4::Identity() : Mat4::Zero();
        float totalWeight = 0;

        for (uint32_t b = 0; b < maxBoneCount; ++b)
        {
            float weight = mesh->boneWeights[i][b];
            Mat4 influence = boneMatrices[mesh->boneIndices[i][b]] * weight;

            if (mode == BoneLinkMode::Additive)
            {
                float inv_weight = 1.0f - weight;
                influence.m11 += inv_weight;
                influence.m22 += inv_weight;
                influence.m33 += inv_weight;
                influence.m44 += inv_weight;

                deformation = influence * deformation;
                totalWeight = 1.0f;
            }
            else // link_mode::normalize or link_mode::total_one
            {
                deformation += influence;
                totalWeight += weight;
            }
        }

        blendedVerts[i] = Vec4(mesh->vertices[i], 1.0f) * deformation;
        blendedNorms[i] = Vec4(mesh->normals[i], 0.0f) * deformation;

        if (mode == BoneLinkMode::Normalize)
        {
            blendedVerts[i] /= totalWeight;
            blendedNorms[i] /= totalWeight;
        }
        else if (mode == BoneLinkMode::TotalOne)
        {
            blendedVerts[i] *= (1.0f - totalWeight);
            blendedNorms[i] *= (1.0f - totalWeight);
        }
    }

    mesh->vertexBuffer->UpdateSubData(0, std::span<std::byte>((std::byte*)blendedVerts.data(), blendedVerts.size() * sizeof(Vec3)));
    mesh->normalBuffer->UpdateSubData(0, std::span<std::byte>((std::byte*)blendedNorms.data(), blendedNorms.size() * sizeof(Vec3)));
}

void MeshRenderer::ToJson(json& obj) const
{
    Component::ToJson(obj);

    ObjectLinker::SaveAsset(obj, "mesh", mesh);

    auto matsIDs = json::array();
    for (auto& mat : materials)
    {
        json matID;
        ObjectLinker::SaveAsset(matID, mat);
        matsIDs.push_back(matID);
    }
    obj["materials"] = matsIDs;

    ObjectLinker::SaveLink(obj, "rootBone", rootBone.lock());
}

void MeshRenderer::FromJson(const json& obj, ObjectLinker* linker)
{
    Component::FromJson(obj, linker);

    ObjectLinker::RestoreAsset(linker, SharedFrom(this), mesh, obj, "mesh");

    auto& matIDs = obj["materials"];
    
    materials.clear();
    materials.resize(matIDs.size());

    for(size_t i = 0; i != materials.size(); ++i)
    {
        UUID uuid = matIDs[i];
        ObjectLinker::RestoreAsset(linker, SharedFrom(this), materials[i], uuid);
    }

    ObjectLinker::RestoreLink(linker, SharedFrom(this), rootBone, obj, "rootBone");
}

void MeshRenderer::GetRenderables(Sink<sptr<Renderable>> sink)
{
    if (!mesh)
        return;

    assert(mesh->vertexBuffer);

    sptr<Node> node = GetNode();
    Mat4 mtxModel = node->GetLocalToWorldMatrix();
    Mat4 mtxNormal = mtxModel.Inverse().Transposed();

    auto bounds = mesh->bbox.Transform(mtxModel);

    renderables.resize(mesh->elements.size());

    for (int i = 0; i != mesh->elements.size(); ++i)
    {
        if (i == materials.size())
            break;

        auto& elem = mesh->elements[i];
        auto& mat = materials[i];

        if(!renderables[i])
            renderables[i] = spnew<Renderable>();
        
        sptr<Renderable> renderable = renderables[i];

        renderable->vertexMapping = {
            { Semantic::POSITION, 0, mesh->vertexBuffer, 0, sizeof(Vec3) },
            { Semantic::NORMAL, 0, mesh->normalBuffer, 0, sizeof(Vec3) },
            { Semantic::TEXCOORD, 0, mesh->texcoordBuffer, 0, sizeof(Vec2) }
        };

        renderable->layerMask = node->GetLayerMask();
        renderable->material = mat;
        renderable->mtxModel = mtxModel;
        renderable->bounds = bounds;
        renderable->extra.SetUniform("uMtxNormal", mtxNormal);
        renderable->indexBuffer = elem.indexBuffer;
        renderable->drawStart = 0;
        renderable->drawCount = elem.indices.size();
        renderable->drawMode = elem.drawMode;

        sink.Add(renderable);
    }
}

} // scene
} // mw
