/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Graphics/Mesh.h>
#include <MW/Graphics/Material.h>
#include <MW/Math/Math.h>
#include <MW/Scene/Components/Collider.h>
#include <MW/Scene/Events.h>

namespace mw {
inline namespace scene {

class SphereCollider : public Collider, public IRenderEvents
{
    REGISTER_OBJECT(SphereCollider);
    friend class PhysicsWorld;

    float radius = 0.5f;

    sptr<Mesh> gizmoMesh;
    sptr<Material> gizmoMat;
    sptr<Renderable> renderable;
public:

    SphereCollider();
    ~SphereCollider();

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
