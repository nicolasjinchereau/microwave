/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.SceneGraph.Components.BoxCollider;
import Microwave.Graphics.Mesh;
import Microwave.Math;
import Microwave.SceneGraph.Components.Collider;
import Microwave.SceneGraph.Events;
import Microwave.SceneGraph.Renderable;
import Microwave.System.Json;
import Microwave.System.Pointers;
import Microwave.Utilities.Sink;

export namespace mw {
inline namespace scene {

class PhysicsWorld;

class BoxCollider : public Collider, public IRenderEvents
{
    inline static Type::Pin<BoxCollider> pin;
    friend class PhysicsWorld;

    Vec3 extents = Vec3(0.5f, 0.5f, 0.5f);

    sptr<Mesh> gizmoMesh;
    sptr<Material> gizmoMat;
    sptr<Renderable> renderable;
public:

    BoxCollider(){}
    ~BoxCollider(){}

    virtual void ToJson(json& obj) const override;
    virtual void FromJson(const json& obj, ObjectLinker* linker) override;

    void SetExtents(const Vec3& extents);
    Vec3 GetExtents() const;

    void FitToMesh(bool searchChildren);

    void UpdateGizmo();

    virtual void GetRenderables(Sink<sptr<Renderable>> sink) override;
};

} // scene
} // mw
