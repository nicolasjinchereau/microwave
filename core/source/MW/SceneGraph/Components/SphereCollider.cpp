/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.SceneGraph.Components.SphereCollider;
import Microwave.SceneGraph.Components.Camera;
import Microwave.SceneGraph.Components.Collider;
import Microwave.SceneGraph.Components.MeshRenderer;
import Microwave.SceneGraph.Components.RigidBody;
import Microwave.SceneGraph.Node;
import Microwave.SceneGraph.Scene;
import Microwave.System.App;

namespace mw {
inline namespace scene {

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
    gptr<MeshRenderer> mr = searchChildren ?
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

void SphereCollider::GetRenderables(Sink<gptr<Renderable>> sink)
{
    if (!GetScene()->GetGizmosEnabled())
        return;

    UpdateGizmo();

    gptr<Node> node = GetNode();
    gptr<Node> pivot = GetPivot();

    Mat4 mtxGizmo = Mat4::Scale(radius, radius, radius);
    Mat4 mtxModel = mtxGizmo * pivot->GetLocalToWorldMatrix();

    auto bounds = gizmoMesh->bbox.Transform(mtxModel);

    if (!renderable)
        renderable = gpnew<Renderable>();

    renderable->vertexMapping = {
        { Semantic::POSITION, 0, gizmoMesh->vertexBuffer, 0, sizeof(Vec3) }
    };

    renderable->layerMask = node->GetLayerMask();
    renderable->material = gizmoMat;
    renderable->mtxModel = mtxModel;
    renderable->bounds = bounds;
    renderable->extra.SetUniform("uColor", Color::Green());
    renderable->indexBuffer = gizmoMesh->elements[0].indexBuffer;
    renderable->drawStart = 0;
    renderable->drawCount = gizmoMesh->elements[0].indices.size();
    renderable->drawMode = gizmoMesh->elements[0].drawMode;

    sink.Add(renderable);
}

} // scene
} // mw
