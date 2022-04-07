/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Graphics/DrawBuffer.h>
#include <MW/Graphics/Mesh.h>
#include <MW/Math/Math.h>
#include <MW/Scene/Components/Collider.h>
#include <MW/Scene/Events.h>

namespace mw {
inline namespace scene {

class PhysicsWorld;

class BoxCollider : public Collider, public IRenderEvents
{
    REGISTER_OBJECT(BoxCollider);
    friend class PhysicsWorld;

    Vec3 extents = Vec3(0.5f, 0.5f, 0.5f);

    sptr<Mesh> gizmoMesh;
    sptr<Material> gizmoMat;
    sptr<Renderable> renderable;
public:

    BoxCollider();
    ~BoxCollider();

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
