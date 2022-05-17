/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.SceneGraph.Components.Collider;
import Microwave.SceneGraph.Components.Component;
import Microwave.System.Json;
import Microwave.System.Object;
import Microwave.System.Pointers;

export namespace mw {
inline namespace scene {

class Node;

class Collider : public Component
{
protected:
    wptr<Node> pivot;
    bool dirty = false;
public:
    Collider(){}
    ~Collider(){}

    virtual void ToJson(json& obj) const override;
    virtual void FromJson(const json& obj, ObjectLinker* linker) override;

    void SetPivot(const sptr<Node>& pivot);
    sptr<Node> GetPivot();

    void SetDirty(bool dirty) { this->dirty = dirty; }
    bool IsDirty() const { return dirty; }
};

} // scene
} // mw
