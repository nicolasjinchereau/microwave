/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/Graphics/GizmoUtility.h>
#include <MW/Scene/Components/Camera.h>
#include <MW/Scene/Components/MeshRenderer.h>
#include <MW/Scene/Components/RigidBody.h>
#include <MW/Scene/Components/SphereCollider.h>
#include <MW/Scene/Node.h>
#include <MW/Scene/Scene.h>
#include <MW/System/App.h>

namespace mw {
inline namespace scene {

SphereCollider::SphereCollider()
{
}

SphereCollider::~SphereCollider()
{
}

void SphereCollider::ToJson(json& obj) const
{
    Collider::ToJson(obj);
    obj["radius"] = radius;
}

void SphereCollider::FromJson(const json& obj, ObjectLinker* linker)
{
    Collider::FromJson(obj, linker);
    radius = obj.value("radius", radius);
    dirty = true;
}

void SphereCollider::SetRadius(float radius)
{
    this->radius = radius;
    dirty = true;
}

float SphereCollider::GetRadius() const {
    return radius;
}

void SphereCollider::FitToMesh(bool searchChildren)
{
    auto node = GetNode();
    sptr<MeshRenderer> mr = searchChildren ?
        node->FindComponentDownward<MeshRenderer>() :
        node->GetComponent<MeshRenderer>();

    if (mr && mr->mesh)
    {
        auto mtx =
            mr->GetNode()->GetLocalToWorldMatrix() *
            node->GetWorldToLocalMatrix();

        Sphere bsphere = mr->mesh->bsphere.Transform(mtx);
        GetPivot()->SetLocalPosition(bsphere.center);
        GetPivot()->SetLocalRotation(Quat::Identity());
        SetRadius(bsphere.radius);
    }
}

void SphereCollider::UpdateGizmo()
{
    auto assetLib = App::Get()->GetAssetLibrary();

    if (!gizmoMesh)
        gizmoMesh = assetLib->GetAsset<Mesh>(".internal/sphere-gizmo.mesh");

    if (!gizmoMat)
        gizmoMat = assetLib->GetAsset<Material>(".internal/wire-gizmo.mat");
}

void SphereCollider::GetRenderables(Sink<sptr<Renderable>> sink)
{
    if (!GetScene()->GetGizmosEnabled())
        return;

    UpdateGizmo();

    sptr<Node> node = GetNode();
    sptr<Node> pivot = GetPivot();

    Mat4 mtxGizmo = Mat4::Scale(radius, radius, radius);
    Mat4 mtxModel = mtxGizmo * pivot->GetLocalToWorldMatrix();

    auto bounds = gizmoMesh->bbox.Transform(mtxModel);

    if (!renderable)
        renderable = spnew<Renderable>();

    renderable->layerMask = node->GetLayerMask();
    renderable->material = gizmoMat;
    renderable->mtxModel = mtxModel;
    renderable->bounds = bounds;
    renderable->extra.SetUniform("uColor", Color::Green());
    renderable->vertexBuffer = gizmoMesh->vertexBuffer;
    renderable->indexBuffer = gizmoMesh->elements[0].indexBuffer;
    renderable->drawStart = 0;
    renderable->drawCount = gizmoMesh->elements[0].indices.size();
    renderable->drawMode = gizmoMesh->elements[0].drawMode;

    sink.Add(renderable);
}

} // scene
} // mw
