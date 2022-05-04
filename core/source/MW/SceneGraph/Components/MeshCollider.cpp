/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.SceneGraph.Components.MeshCollider;
import Microwave.Graphics.Shader;
import Microwave.SceneGraph.Components.Camera;
import Microwave.SceneGraph.Components.MeshRenderer;
import Microwave.SceneGraph.Components.RigidBody;
import Microwave.SceneGraph.Node;
import Microwave.SceneGraph.Scene;
import Microwave.System.App;

namespace mw {
inline namespace scene {

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

    renderable->vertexMapping = {
        { Semantic::POSITION, 0, mesh->vertexBuffer, 0, sizeof(Vec3) }
    };

    renderable->layerMask = node->GetLayerMask();
    renderable->material = gizmoMat;
    renderable->mtxModel = mtxModel;
    renderable->bounds = bounds;
    renderable->extra.SetUniform("uColor", Color::Green());
    renderable->indexBuffer = mesh->elements[0].indexBuffer;
    renderable->drawStart = 0;
    renderable->drawCount = mesh->elements[0].indices.size();
    renderable->drawMode = mesh->elements[0].drawMode;

    sink.Add(renderable);
}

} // scene
} // mw
