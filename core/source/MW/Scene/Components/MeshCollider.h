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

class MeshCollider : public Collider, public IRenderEvents
{
    REGISTER_OBJECT(MeshCollider);
    friend class PhysicsWorld;

    sptr<Mesh> mesh;
    sptr<Material> gizmoMat;
    sptr<Renderable> renderable;
public:

    MeshCollider();
    ~MeshCollider();

    virtual void ToJson(json& obj) const override;
    virtual void FromJson(const json& obj, ObjectLinker* linker) override;

    void SetMesh(const sptr<Mesh>& mesh);
    sptr<Mesh> GetMesh() const;

    void UpdateGizmo();

    virtual void GetRenderables(Sink<sptr<Renderable>> sink) override;
};

} // scene
} // mw
