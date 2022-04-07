/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Graphics/Mesh.h>
#include <MW/Graphics/Material.h>
#include <MW/Scene/Components/Component.h>
#include <MW/Scene/Events.h>

namespace mw {
inline namespace scene {

class Camera;

class MeshRenderer :
    public Component,
    public IRenderEvents,
    public ISystemEvents
{
    REGISTER_OBJECT(MeshRenderer);
public:
    sptr<Mesh> mesh;
    std::vector<sptr<Material>> materials;
    wptr<Node> rootBone;

    AABox GetBounds() const;

    virtual void SystemLateUpdate() override;

    virtual void ToJson(json& obj) const override;
    virtual void FromJson(const json& obj, ObjectLinker* linker) override;

    virtual void GetRenderables(Sink<sptr<Renderable>> sink) override;
private:
    std::vector<Mat4> boneMatrices;
    std::vector<Vec3> blendedVerts;
    std::vector<Vec3> blendedNorms;

    std::vector<sptr<Renderable>> renderables;
};

}
}
