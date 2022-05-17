/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.SceneGraph.Components.SphereCollider;
import Microwave.Graphics.Material;
import Microwave.Graphics.Mesh;
import Microwave.Math;
import Microwave.SceneGraph.Events;
import Microwave.SceneGraph.Renderable;
import Microwave.SceneGraph.Components.Collider;
import Microwave.System.Json;
import Microwave.System.Object;
import Microwave.System.Pointers;
import Microwave.Utilities.Sink;

export namespace mw {
inline namespace scene {

class PhysicsWorld;

class SphereCollider : public Collider, public IRenderEvents
{
    inline static Type::Pin<SphereCollider> pin;

    friend PhysicsWorld;

    float radius = 0.5f;

    sptr<Mesh> gizmoMesh;
    sptr<Material> gizmoMat;
    sptr<Renderable> renderable;
public:

    SphereCollider(){}
    ~SphereCollider(){}

    virtual void ToJson(json& obj) const override;
    virtual void FromJson(const json& obj, ObjectLinker* linker) override;

    void SetRadius(float radius);
    float GetRadius() const;

    void FitToMesh(bool searchChildren);

    void UpdateGizmo();

    virtual void GetRenderables(Sink<sptr<Renderable>> sink) override;
};

} // scene
} // mw
