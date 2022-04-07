/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/System/Json.h>
#include <MW/Graphics/GizmoUtility.h>
#include <MW/Scene/Node.h>
#include <MW/Scene/Scene.h>
#include <MW/Scene/Components/MeshCollider.h>
#include <MW/Scene/Components/RigidBody.h>
#include <MW/Scene/Components/Camera.h>
#include <MW/Scene/Components/MeshRenderer.h>
#include <MW/System/App.h>

namespace mw {
inline namespace scene {

MeshCollider::MeshCollider()
{    
}

MeshCollider::~MeshCollider()
{
}

void MeshCollider::ToJson(json& obj) const
{
    Collider::ToJson(obj);
    obj["mesh"] = mesh;
}

void MeshCollider::FromJson(const json& obj, ObjectLinker* linker)
{
    Collider::FromJson(obj, linker);
    mesh = obj.value("mesh", mesh);
    dirty = true;
}

void MeshCollider::SetMesh(const sptr<Mesh>& mesh)
{
    this->mesh = mesh;
    dirty = true;
}

sptr<Mesh> MeshCollider::GetMesh() const {
    return mesh;
}

void MeshCollider::UpdateGizmo()
{
    auto assetLib = App::Get()->GetAssetLibrary();

    if (!gizmoMat)
        gizmoMat = assetLib->GetAsset<Material>(".internal/wire-gizmo.mat");
}

void MeshCollider::GetRenderables(Sink<sptr<Renderable>> sink)
{
    if (!GetScene()->GetGizmosEnabled())
        return;

    if (!mesh)
        return;

    UpdateGizmo();

    sptr<Node> node = GetNode();
    sptr<Node> pivot = GetPivot();

    Mat4 mtxModel = pivot->GetLocalToWorldMatrix();
    AABox bounds = mesh->bbox.Transform(mtxModel);

    if (!renderable)
        renderable = spnew<Renderable>();

    renderable->layerMask = node->GetLayerMask();
    renderable->material = gizmoMat;
    renderable->mtxModel = mtxModel;
    renderable->bounds = bounds;
    renderable->extra.SetUniform("uColor", Color::Green());
    renderable->vertexBuffer = mesh->vertexBuffer;
    renderable->indexBuffer = mesh->elements[0].indexBuffer;
    renderable->drawStart = 0;
    renderable->drawCount = mesh->elements[0].indices.size();
    renderable->drawMode = mesh->elements[0].drawMode;

    sink.Add(renderable);
}

} // scene
} // mw
