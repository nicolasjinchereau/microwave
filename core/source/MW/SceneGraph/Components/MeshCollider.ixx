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

class MeshCollider : public Collider, public IRenderEvents
{
    inline static Type::Pin<MeshCollider> pin;

    sptr<Mesh> mesh;
    sptr<Material> gizmoMat;
    sptr<Renderable> renderable;

    friend class PhysicsWorld;
public:

    MeshCollider(){}
    ~MeshCollider(){}

    virtual void ToJson(json& obj) const override;
    virtual void FromJson(const json& obj, ObjectLinker* linker) override;

    void SetMesh(const sptr<Mesh>& mesh);
    sptr<Mesh> GetMesh() const;

    void UpdateGizmo();

    virtual void GetRenderables(Sink<sptr<Renderable>> sink) override;
};

} // scene
} // mw
