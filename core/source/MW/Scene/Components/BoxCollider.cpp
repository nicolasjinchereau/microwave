/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/Graphics/GizmoUtility.h>
#include <MW/System/App.h>
#include <MW/Scene/Node.h>
#include <MW/Scene/Components/Camera.h>
#include <MW/Scene/Components/MeshRenderer.h>
#include <MW/Scene/Components/BoxCollider.h>
#include <MW/Scene/Components/RigidBody.h>
#include <MW/Scene/Scene.h>
#include <MW/System/App.h>

namespace mw {
inline namespace scene {

BoxCollider::BoxCollider()
{
}

BoxCollider::~BoxCollider()
{
}

void BoxCollider::ToJson(json& obj) const
{
    Collider::ToJson(obj);
    obj["extents"] = extents;
}

void BoxCollider::FromJson(const json& obj, ObjectLinker* linker)
{
    Collider::FromJson(obj, linker);
    extents = obj.value("extents", extents);
    dirty = true;
}

void BoxCollider::SetExtents(const Vec3& extents)
{
    this->extents = extents;
    dirty = true;
}

Vec3 BoxCollider::GetExtents() const {
    return extents;
}

void BoxCollider::FitToMesh(bool searchChildren)
{
    auto node = GetNode();
    sptr<MeshRenderer> mr =  searchChildren ? 
        node->FindComponentDownward<MeshRenderer>() :
        node->GetComponent<MeshRenderer>();
    
    if (mr && mr->mesh)
    {
        auto mtx =
            mr->GetNode()->GetLocalToWorldMatrix() *
            node->GetWorldToLocalMatrix();

        AABox bbox = mr->mesh->bbox.Transform(mtx);
        GetPivot()->SetLocalPosition(bbox.center);
        GetPivot()->SetLocalRotation(Quat::Identity());
        SetExtents(bbox.extents);
    }
}

void BoxCollider::UpdateGizmo()
{
    auto assetLib = App::Get()->GetAssetLibrary();

    if (!gizmoMesh)
        gizmoMesh = assetLib->GetAsset<Mesh>(".internal/box-gizmo.mesh");

    if (!gizmoMat)
        gizmoMat = assetLib->GetAsset<Material>(".internal/wire-gizmo.mat");
}

void BoxCollider::GetRenderables(Sink<sptr<Renderable>> sink)
{
    if(!GetScene()->GetGizmosEnabled())
        return;

    UpdateGizmo();

    sptr<Node> node = GetNode();
    sptr<Node> pivot = GetPivot();

    Mat4 mtxGizmo = Mat4::Scale(extents);
    Mat4 mtxModel = mtxGizmo * pivot->GetLocalToWorldMatrix();

    auto bounds = gizmoMesh->bbox.Transform(mtxModel);

    if(!renderable)
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
