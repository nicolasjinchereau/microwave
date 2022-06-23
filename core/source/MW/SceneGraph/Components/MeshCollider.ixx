/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.SceneGraph.Components.MeshCollider;
import Microwave.Graphics.Material;
import Microwave.Graphics.Mesh;
import Microwave.Math;
import Microwave.SceneGraph.Components.Collider;
import Microwave.SceneGraph.Events;
import Microwave.SceneGraph.Renderable;
import Microwave.System.Json;
import Microwave.System.Object;
import Microwave.System.Pointers;
import Microwave.Utilities.Sink;

export namespace mw {
inline namespace scene {

class PhysicsWorld;

class MeshCollider : public Collider, public IRenderEvents
{
    inline static Type::Pin<MeshCollider> pin;

    gptr<Mesh> mesh;
    gptr<Material> gizmoMat;
    gptr<Renderable> renderable;

    friend PhysicsWorld;
public:

    MeshCollider(){}
    ~MeshCollider(){}

    virtual void ToJson(json& obj) const override;
    virtual void FromJson(const json& obj, ObjectLinker* linker) override;

    void SetMesh(const gptr<Mesh>& mesh);
    gptr<Mesh> GetMesh() const;

    void UpdateGizmo();

    virtual void GetRenderables(Sink<gptr<Renderable>> sink) override;
};

} // scene
} // mw
