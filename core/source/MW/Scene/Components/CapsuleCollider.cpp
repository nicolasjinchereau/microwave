/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/Graphics/GizmoUtility.h>
#include <MW/Scene/Node.h>
#include <MW/Scene/Scene.h>
#include <MW/Scene/Components/Camera.h>
#include <MW/Scene/Components/CapsuleCollider.h>
#include <MW/Scene/Components/MeshRenderer.h>
#include <MW/Scene/Components/RigidBody.h>
#include <MW/System/App.h>

namespace mw {
inline namespace scene {

CapsuleCollider::CapsuleCollider()
{
}

CapsuleCollider::~CapsuleCollider()
{
}

void CapsuleCollider::ToJson(json& obj) const
{
    Collider::ToJson(obj);
    obj["upAxis"] = upAxis;
    obj["radius"] = radius;
    obj["height"] = height;
}

void CapsuleCollider::FromJson(const json& obj, ObjectLinker* linker)
{
    Collider::FromJson(obj, linker);
    upAxis = obj.value("upAxis", upAxis);
    radius = obj.value("radius", radius);
    height = obj.value("height", height);
    dirty = true;
}

void CapsuleCollider::SetUpAxis(Axis upAxis)
{
    this->upAxis = upAxis;
    dirty = true;
}

Axis CapsuleCollider::GetUpAxis() const
{
    return upAxis;
}

void CapsuleCollider::SetRadius(float radius)
{
    this->radius = radius;
    dirty = true;
}

float CapsuleCollider::GetRadius() const {
    return radius;
}

void CapsuleCollider::SetHeight(float height)
{
    this->height = height;
    dirty = true;
}

float CapsuleCollider::GetHeight() const {
    return height;
}

void CapsuleCollider::FitToMesh(bool searchChildren)
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

        AABox bbox = mr->mesh->bbox.Transform(mtx);

        GetPivot()->SetLocalPosition(bbox.center);
        GetPivot()->SetLocalRotation(Quat::Identity());

        if (upAxis == Axis::X)
        {
            radius = (bbox.extents.y + bbox.extents.z) * 0.5f;
            height = std::max(bbox.extents.x * 2.0f - radius * 2.0f, 0.0f);
        }
        else if (upAxis == Axis::Y)
        {
            radius = (bbox.extents.x + bbox.extents.z) * 0.5f;
            height = std::max(bbox.extents.y * 2.0f - radius * 2.0f, 0.0f);
        }
        else if (upAxis == Axis::Z)
        {
            radius = (bbox.extents.y + bbox.extents.z) * 0.5f;
            height = std::max(bbox.extents.z * 2.0f - radius * 2.0f, 0.0f);
        }
        
        dirty = true;
    }
}

void CapsuleCollider::UpdateGizmo()
{
    auto assetLib = App::Get()->GetAssetLibrary();

    if (!gizmoMat)
        gizmoMat = assetLib->GetAsset<Material>(".internal/wire-gizmo.mat");

    if (!gizmoCapMesh)
        gizmoCapMesh = assetLib->GetAsset<Mesh>(".internal/half-sphere-gizmo.mesh");

    if (!gizmoBodyMesh)
        gizmoBodyMesh = assetLib->GetAsset<Mesh>(".internal/cylinder-gizmo.mesh");

    Vec3 capOffset(0, height * 0.5f, 0);
    Mat4 upRotation;
        
    if (upAxis == Axis::X)
        upRotation = Mat4::Rotation(0, 0, 90);
    else if (upAxis == Axis::Y)
        upRotation = Mat4::Rotation(0, 0, 0);
    else if (upAxis == Axis::Z)
        upRotation = Mat4::Rotation(90, 0, 0);

    mtxCapTop =
        Mat4::Scale(radius, radius, radius) *
        Mat4::Translation(capOffset) *
        upRotation;

    mtxBody =
        Mat4::Scale(radius, height, radius) *
        upRotation;

    mtxCapBot =
        Mat4::Scale(radius, -radius, radius) *
        Mat4::Translation(-capOffset) *
        upRotation;
}

void CapsuleCollider::GetRenderables(Sink<sptr<Renderable>> sink)
{
    if (!GetScene()->GetGizmosEnabled())
        return;

    UpdateGizmo();

    sptr<Node> node = GetNode();
    sptr<Node> pivot = GetPivot();
    
    Mat4 mtxModel = pivot->GetLocalToWorldMatrix();

    Mat4 mtxModelTop = mtxCapTop * mtxModel;
    Mat4 mtxModelMid = mtxBody * mtxModel;
    Mat4 mtxModelBot = mtxCapBot * mtxModel;

    AABox boundsTop = gizmoCapMesh->bbox.Transform(mtxModelTop);
    AABox boundsBody = gizmoBodyMesh->bbox.Transform(mtxModelMid);
    AABox boundsBot = gizmoCapMesh->bbox.Transform(mtxModelBot);
    
    renderables.resize(3);

    for (int i = 0; i != 3; ++i)
    {
        if (!renderables[i])
            renderables[i] = spnew<Renderable>();
    }

    // top
    renderables[0]->layerMask = node->GetLayerMask();
    renderables[0]->material = gizmoMat;
    renderables[0]->mtxModel = mtxModelTop;
    renderables[0]->bounds = boundsTop;
    renderables[0]->extra.SetUniform("uColor", Color::Green());
    renderables[0]->vertexBuffer = gizmoCapMesh->vertexBuffer;
    renderables[0]->indexBuffer = gizmoCapMesh->elements[0].indexBuffer;
    renderables[0]->drawStart = 0;
    renderables[0]->drawCount = gizmoCapMesh->elements[0].indices.size();
    renderables[0]->drawMode = gizmoCapMesh->elements[0].drawMode;
    sink.Add(renderables[0]);

    // body
    renderables[1]->layerMask = node->GetLayerMask();
    renderables[1]->material = gizmoMat;
    renderables[1]->mtxModel = mtxModelMid;
    renderables[1]->bounds = boundsBody;
    renderables[1]->extra.SetUniform("uColor", Color::Green());
    renderables[1]->vertexBuffer = gizmoBodyMesh->vertexBuffer;
    renderables[1]->indexBuffer = gizmoBodyMesh->elements[0].indexBuffer;
    renderables[1]->drawStart = 0;
    renderables[1]->drawCount = gizmoBodyMesh->elements[0].indices.size();
    renderables[1]->drawMode = gizmoBodyMesh->elements[0].drawMode;
    sink.Add(renderables[1]);
    
    // bottom
    renderables[2]->layerMask = node->GetLayerMask();
    renderables[2]->material = gizmoMat;
    renderables[2]->mtxModel = mtxModelBot;
    renderables[2]->bounds = boundsBot;
    renderables[2]->extra.SetUniform("uColor", Color::Green());
    renderables[2]->vertexBuffer = gizmoCapMesh->vertexBuffer;
    renderables[2]->indexBuffer = gizmoCapMesh->elements[0].indexBuffer;
    renderables[2]->drawStart = 0;
    renderables[2]->drawCount = gizmoCapMesh->elements[0].indices.size();
    renderables[2]->drawMode = gizmoCapMesh->elements[0].drawMode;
    sink.Add(renderables[2]);
}

} // scene
} // mw
