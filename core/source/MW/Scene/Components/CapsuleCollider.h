/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Graphics/Mesh.h>
#include <MW/Graphics/Material.h>
#include <MW/Math/Math.h>
#include <MW/Scene/Components/Collider.h>
#include <MW/Scene/Events.h>
#include <MW/Scene/Axis.h>

namespace mw {
inline namespace scene {

class CapsuleShape;

class CapsuleCollider : public Collider, public IRenderEvents
{
    REGISTER_OBJECT(CapsuleCollider);
    friend class PhysicsWorld;

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
public:

    CapsuleCollider();
    ~CapsuleCollider();

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
