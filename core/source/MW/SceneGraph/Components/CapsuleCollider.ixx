/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.SceneGraph.Components.CapsuleCollider;
import Microwave.Graphics.Mesh;
import Microwave.Graphics.Material;
import Microwave.Math;
import Microwave.SceneGraph.Components.Collider;
import Microwave.SceneGraph.Axis;
import Microwave.SceneGraph.Events;
import Microwave.SceneGraph.Renderable;
import Microwave.System.Json;
import Microwave.System.Pointers;
import Microwave.Utilities.Sink;
import <vector>;

export namespace mw {
inline namespace scene {

class CapsuleShape;
class PhysicsWorld;

class CapsuleCollider : public Collider, public IRenderEvents
{
    inline static Type::Pin<CapsuleCollider> pin;

    Axis upAxis = Axis::Y;
    float radius = 0.5f;
    float height = 1.0f;

    sptr<Mesh> gizmoBodyMesh;
    sptr<Mesh> gizmoCapMesh;
    Mat4 mtxCapTop;
    Mat4 mtxBody;
    Mat4 mtxCapBot;
    sptr<Material> gizmoMat;
    std::vector<sptr<Renderable>> renderables;

    friend PhysicsWorld;
public:

    CapsuleCollider(){}
    ~CapsuleCollider(){}

    virtual void ToJson(json& obj) const override;
    virtual void FromJson(const json& obj, ObjectLinker* linker) override;

    void SetUpAxis(Axis upAxis);
    Axis GetUpAxis() const;

    void SetRadius(float radius);
    float GetRadius() const;

    // height of middle section between caps
    void SetHeight(float height);
    float GetHeight() const;

    void FitToMesh(bool searchChildren);

    void UpdateGizmo();

    virtual void GetRenderables(Sink<sptr<Renderable>> sink) override;
};

} // scene
} // mw
