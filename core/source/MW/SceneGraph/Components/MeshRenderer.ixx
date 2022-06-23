/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.SceneGraph.Components.MeshRenderer;
import Microwave.Graphics.Material;
import Microwave.Graphics.Mesh;
import Microwave.Math;
import Microwave.SceneGraph.Components.Component;
import Microwave.SceneGraph.Events;
import Microwave.SceneGraph.Renderable;
import Microwave.System.Json;
import Microwave.System.Object;
import Microwave.System.Pointers;
import Microwave.Utilities.Sink;
import <cstdint>;
import <vector>;

export namespace mw {
inline namespace scene {

class Camera;
class Node;

class MeshRenderer :
    public Component,
    public IRenderEvents,
    public ISystemEvents
{
    inline static Type::Pin<MeshRenderer> pin;
public:
    gptr<Mesh> mesh;
    gvector<gptr<Material>> materials;
    wgptr<Node> rootBone;
    
    MeshRenderer(){}
    ~MeshRenderer(){}

    AABox GetBounds() const;

    virtual void SystemLateUpdate() override;

    virtual void ToJson(json& obj) const override;
    virtual void FromJson(const json& obj, ObjectLinker* linker) override;

    virtual void GetRenderables(Sink<gptr<Renderable>> sink) override;
private:
    std::vector<Mat4> boneMatrices;
    std::vector<Vec3> blendedVerts;
    std::vector<Vec3> blendedNorms;

    gvector<gptr<Renderable>> renderables;
};

}
}
