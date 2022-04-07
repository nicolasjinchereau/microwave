/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Data/Database/AssetDatabase.h>
#include <MW/Data/Database/ModelImporter.h>
#include <MW/Data/Library/AssetLibrary.h>
#include <MW/Data/Internal/FBXModelConverter.h>
#include <MW/Scene/Components/Animator.h>
#include <MW/Scene/Components/MeshRenderer.h>
#include <MW/Scene/Components/SphereCollider.h>
#include <MW/Scene/Components/BoxCollider.h>
#include <MW/Scene/Components/CapsuleCollider.h>
#include <MW/Scene/Components/MeshCollider.h>
#include <MW/Scene/Components/RigidBody.h>
#include <MW/Utilities/Util.h>

namespace mw {
inline namespace data {

struct ModelImporter::ParserState
{
    ModelSettings* settings;
    sptr<Node> rootNode;
    std::unordered_map<std::string, sptr<Mesh>> meshes;
    std::unordered_map<std::string, MaterialInfo> materials;
    std::vector<sptr<AnimationClip>> clips;
    std::vector<sptr<MeshRenderer>> meshRenderers;
};

std::span<std::string> ModelImporter::GetSupportedFileTypes() {
    static std::string types[] = { ".fbx" };
    return types;
}

void ModelImporter::ImportFile(
    AssetMetadata& meta,
    const sptr<Stream>& stream,
    const path& dataDir)
{
    ModelSettings settings = meta.settings;
    
    auto ext = ToLower(meta.sourcePath.extension().string());
    assert(ext == ".fbx");

    sptr<Model> model = FBXModelConverter::Convert(stream, settings);

    ParserState state;
    state.settings = &settings;
    ParseModel(model, state);
    
    // save all generated asset artifacts
    std::unordered_map<std::string, ArtifactMetadata> artifacts;
    std::unordered_map<std::string, sptr<Object>> objects;

    auto RetainOldUUID = [&](ArtifactMetadata& newArt, const sptr<Object>& obj)
    {
        auto it = std::find_if(
            meta.artifacts.begin(),
            meta.artifacts.end(),
            [&](const ArtifactMetadata& oldArt) {
                return oldArt.sourcePath == newArt.sourcePath;
            }
        );

        if (it != meta.artifacts.end())
        {
            newArt.uuid = it->uuid;
            obj->SetUUID(it->uuid);
        }
    };

    // model root node
    {
        ArtifactMetadata art;
        art.uuid = state.rootNode->GetUUID();
        art.sourcePath = meta.sourcePath;
        art.assetType = AssetType::Node;
        RetainOldUUID(art, state.rootNode);

        artifacts[art.sourcePath] = art;
        objects[art.sourcePath] = state.rootNode;
    }

    for (auto& [name, mesh] : state.meshes)
    {
        ArtifactMetadata art;
        art.uuid = mesh->GetUUID();
        art.sourcePath = meta.sourcePath / mesh->GetName() + ".mesh";
        art.assetType = AssetType::Mesh;
        RetainOldUUID(art, mesh);

        artifacts[art.sourcePath] = art;
        objects[art.sourcePath] = mesh;
    }
    
    settings.materialSettings.clear();

    for (auto& [name, minfo] : state.materials)
    {
        ArtifactMetadata art;
        art.uuid = minfo.material->GetUUID();
        art.sourcePath = meta.sourcePath / minfo.material->GetName() + ".mat";
        art.assetType = AssetType::Material;
        RetainOldUUID(art, minfo.material);

        artifacts[art.sourcePath] = art;
        objects[art.sourcePath] = minfo.material;

        auto& matSettings = settings.materialSettings[art.uuid];
        matSettings.shaderBinding = minfo.shaderBinding;
        matSettings.textureBindings = minfo.textureBindings;
    }

    for (auto& clip : state.clips)
    {
        ArtifactMetadata art;
        art.uuid = clip->GetUUID();
        art.sourcePath = meta.sourcePath / clip->GetName() + ".clip";
        art.assetType = AssetType::AnimationClip;
        RetainOldUUID(art, clip);

        artifacts[art.sourcePath] = art;
        objects[art.sourcePath] = clip;
    }

    meta.artifacts.clear();

    for (auto& [sourcePath, art] : artifacts)
    {
        json obj;
        objects[sourcePath]->ToJson(obj);

        path destFilePath = dataDir / art.uuid.ToString();
        File::WriteAllText(destFilePath, obj.dump(2));
        
        meta.artifacts.push_back(std::move(art));
    }

    meta.settings = settings;
}

bool ModelImporter::Resolve(
    AssetDatabase& assetDatabase,
    AssetMetadata& meta,
    const path& sourceDir,
    const path& dataDir)
{
    bool resolvedReference = false;
    auto assetLib = assetDatabase.GetAssetLibrary();

    ModelSettings modelSettings = meta.settings;

    for (auto& art : meta.artifacts)
    {
        auto it = modelSettings.materialSettings.find(art.uuid);
        if (it != modelSettings.materialSettings.end())
        {
            MaterialSettings& matSettings = it->second;

            sptr<Material> mat;

            for (auto& [uniform, binding] : matSettings.textureBindings)
            {
                if (binding.filename.empty())
                    continue;

                std::optional<UUID> uuid = assetLib->FindAssetUUID(binding.filename);

                if(binding.uuid != uuid)
                {
                    // binding changed - needs update
                    if (!mat)
                    {
                        auto artifactPath = dataDir / art.uuid.ToString();
                        auto txt = File::ReadAllText(artifactPath);
                        mat = ObjectFactory::FromJson<Material>(json::parse(txt), nullptr);
                    }

                    sptr<Texture> tex = assetLib->GetAsset<Texture>(*uuid);
                    mat->SetUniform(uniform, tex);
                    binding.uuid = uuid;
                }
            }
            
            if (!matSettings.shaderBinding.filename.empty())
            {
                std::optional<UUID> uuid = assetLib->FindAssetUUID(
                    matSettings.shaderBinding.filename);

                if (matSettings.shaderBinding.uuid != uuid)
                {
                    // binding changed - needs update
                    if (!mat)
                    {
                        auto artifactPath = dataDir / art.uuid.ToString();
                        auto txt = File::ReadAllText(artifactPath);
                        mat = ObjectFactory::FromJson<Material>(json::parse(txt), nullptr);
                    }

                    mat->shader = assetLib->GetAsset<Shader>(*uuid);
                    matSettings.shaderBinding.uuid = uuid;
                }
            }

            if (mat)
            {
                json obj;
                mat->ToJson(obj);
                auto artifactPath = dataDir / art.uuid.ToString();
                File::WriteAllText(artifactPath, obj.dump(2));

                resolvedReference = true;
            }
        }
    }

    if (resolvedReference)
        meta.settings = modelSettings;

    return resolvedReference;
}

void ModelImporter::ParseModel(const sptr<Model>& model, ParserState& state)
{
    state.rootNode = ParseModelNode(model->rootNode, state);
    
    for (auto& modelClip : model->clips)
    {
        auto clip = spnew<AnimationClip>();
        clip->SetName(modelClip->name);
        clip->SetWrapMode((AnimationWrapMode)modelClip->wrapMode);

        for (auto& [path, track] : modelClip->tracks)
            clip->AddTrack(path, track);

        state.clips.push_back(std::move(clip));
    }

    if (!state.clips.empty())
    {
        auto anim = state.rootNode->AddComponent<Animator>();

        for(auto& clip : state.clips)
            anim->AddClip(clip, clip->GetName());
    }

    for (auto& mr : state.meshRenderers)
    {
        if (mr->mesh->skinType != SkinType::None)
        {
            mr->rootBone = state.rootNode;
        }
    }
}

sptr<Node> ModelImporter::ParseModelNode(const sptr<ModelNode>& modelNode, ParserState& state)
{
    auto node = spnew<Node>();

    node->SetName(modelNode->name);
    node->SetLocalPosition(modelNode->localPosition);
    node->SetLocalScale(modelNode->localScale);
    node->SetLocalRotation(modelNode->localRotation);

    for (auto& c : modelNode->children)
    {
        auto n = ParseModelNode(c, state);
        n->SetParent(node, false);
    }

    if (modelNode->mesh)
    {
        sptr<Mesh> mesh;
        std::vector<sptr<Material>> materials;

        if (auto it = state.meshes.find(modelNode->mesh->name);
            it != state.meshes.end())
        {
            mesh = it->second;
        }
        else
        {
            mesh = spnew<Mesh>();
            mesh->SetName(modelNode->mesh->name);
            mesh->vertices = std::move(modelNode->mesh->vertices);
            mesh->normals = std::move(modelNode->mesh->normals);
            mesh->texcoords = std::move(modelNode->mesh->texcoords);
            mesh->skinType = std::move(modelNode->mesh->skinType);
            mesh->bones = std::move(modelNode->mesh->bones);
            mesh->boneIndices = std::move(modelNode->mesh->boneIndices);
            mesh->boneWeights = std::move(modelNode->mesh->boneWeights);
            mesh->RecalcBounds();

            for (auto& elem : modelNode->mesh->elements)
            {
                MeshElement meshElem;
                meshElem.drawMode = elem.drawMode;
                meshElem.indices = std::move(elem.indices);
                mesh->elements.push_back(meshElem);
            }

            state.meshes[modelNode->mesh->name] = mesh;
        }

        for (auto& elem : modelNode->mesh->elements)
        {
            sptr<Material> mat;

            auto it = state.materials.find(elem.material->name);
            if (it != state.materials.end())
            {
                mat = it->second.material;
            }
            else
            {
                mat = spnew<Material>();
                mat->SetName(elem.material->name);
                mat->colorBlendOperation = elem.material->colorBlendOperation;
                mat->alphaBlendOperation = elem.material->alphaBlendOperation;
                mat->sourceColorBlendFactor = elem.material->sourceColorBlendFactor;
                mat->sourceAlphaBlendFactor = elem.material->sourceAlphaBlendFactor;
                mat->destColorBlendFactor = elem.material->destColorBlendFactor;
                mat->destAlphaBlendFactor = elem.material->destAlphaBlendFactor;
                mat->cullMode = elem.material->cullMode;
                mat->depthTest = elem.material->depthTest;
                mat->depthWriteEnabled = elem.material->depthWriteEnabled;
                mat->renderQueue = elem.material->renderQueue;
                mat->SetUniform("uDiffuseTex_ST", elem.material->uDiffuseTex_ST);
                mat->SetUniform("uDiffuseFactor", elem.material->uDiffuseFactor);
                mat->SetUniform("uDiffuseColor", elem.material->uDiffuseColor);
                mat->SetUniform("uSpecularTex_ST", elem.material->uSpecularTex_ST);
                mat->SetUniform("uSpecularFactor", elem.material->uSpecularFactor);
                mat->SetUniform("uSpecularColor", elem.material->uSpecularColor);
                mat->SetUniform("uShininess", elem.material->uShininess);
                mat->SetUniform("uNormalTex_ST", elem.material->uNormalTex_ST);
                mat->SetUniform("uNormalFactor", elem.material->uNormalFactor);
                
                MaterialInfo minfo;
                minfo.material = mat;
                
                minfo.shaderBinding.filename = "default.cg";

                if(!elem.material->uDiffuseTex.empty())
                {
                    std::string filename { elem.material->uDiffuseTex.filename() };
                    if (!filename.empty())
                        minfo.textureBindings["uDiffuseTex"] = { filename, {} };
                }

                if (!elem.material->uSpecularTex.empty())
                {
                    std::string filename { elem.material->uSpecularTex.filename() };
                    if (!filename.empty())
                        minfo.textureBindings["uSpecularTex"] = { filename, {} };
                }

                if (!elem.material->uNormalTex.empty())
                {
                    std::string filename { elem.material->uNormalTex.filename() };
                    if (!filename.empty())
                        minfo.textureBindings["uNormalTex"] = { filename, {} };
                }

                state.materials[elem.material->name] = std::move(minfo);
            }

            materials.push_back(mat);
        }

        auto mr = node->AddComponent<MeshRenderer>();
        mr->mesh = mesh;
        mr->materials = materials;
        state.meshRenderers.push_back(mr);
    }

    if (modelNode->collider)
    {
        auto colliderType = modelNode->collider->type;
        if (colliderType == ModelColliderType::Sphere)
        {
            auto col = node->AddComponent<SphereCollider>();
            col->SetName(modelNode->collider->name);
            col->SetPivot(node->GetChild(modelNode->collider->pivotID));
            col->SetRadius(modelNode->collider->radius);
        }
        else if (colliderType == ModelColliderType::Box)
        {
            auto col = node->AddComponent<BoxCollider>();
            col->SetName(modelNode->collider->name);
            col->SetPivot(node->GetChild(modelNode->collider->pivotID));
            col->SetExtents({
                modelNode->collider->length * 0.5f,
                modelNode->collider->height * 0.5f,
                modelNode->collider->width * 0.5f
            });
        }
        else if (colliderType == ModelColliderType::Capsule)
        {
            auto col = node->AddComponent<CapsuleCollider>();
            col->SetName(modelNode->collider->name);
            col->SetPivot(node->GetChild(modelNode->collider->pivotID));
            col->SetRadius(modelNode->collider->radius);
            col->SetHeight(modelNode->collider->height);
        }
        else if (colliderType == ModelColliderType::Convex)
        {
            assert(modelNode->collider->mesh);
            sptr<Mesh> mesh = spnew<Mesh>();
            mesh->SetName(modelNode->collider->mesh->name);
            mesh->vertices = std::move(modelNode->collider->mesh->vertices);
            auto& elem = modelNode->collider->mesh->elements[0];
            MeshElement meshElem;
            meshElem.drawMode = elem.drawMode;
            meshElem.indices = std::move(elem.indices);
            mesh->elements.push_back(meshElem);
            mesh->RecalcBounds();

            auto col = node->AddComponent<MeshCollider>();
            col->SetName(modelNode->collider->name);
            col->SetPivot(node->GetChild(modelNode->collider->pivotID));
            col->SetMesh(mesh);
        }

        auto body = node->AddComponent<RigidBody>();
        auto bodyType = modelNode->collider->bodyType;

        if(bodyType == ModelRigidBodyType::Dynamic)
            body->SetBodyType(BodyType::Dynamic);
        else if (bodyType == ModelRigidBodyType::Kinematic)
            body->SetBodyType(BodyType::Kinematic);
        else if (bodyType == ModelRigidBodyType::Static)
            body->SetBodyType(BodyType::Static);

        body->SetFriction(modelNode->collider->friction);
    }

    return node;
}

}
}
