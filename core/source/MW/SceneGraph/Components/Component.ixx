/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.SceneGraph.Components.Component;
import Microwave.SceneGraph.Events;
import Microwave.System.Clock;
import Microwave.System.Json;
import Microwave.System.Object;
import Microwave.System.Pointers;

export namespace mw {
inline namespace scene {

class Node;
class Camera;
class Scene;

class Component : public virtual EventTarget, public Object
{
    inline static Type::Pin<Component> pin;
    friend class Node;
public:

    Component(){}
    virtual ~Component(){}

    sptr<Clock> GetClock();
    sptr<const Clock> GetClock() const;

    virtual void ToJson(json& obj) const override;
    virtual void FromJson(const json& obj, ObjectLinker* linker) override;

    virtual void OnAttachedToScene() {}
    virtual void OnDetachFromScene() {}

    virtual void OnNodeActivated() {}
    virtual void OnNodeDeactivated() {}

    // transform of the owning node was set to dirty
    virtual void OnTransformChanged() {}

    // structure at or below this node has changed
    // by adding/removing a child or component
    virtual void OnStructureChanged() {}

    virtual void Draw(Camera* camera) {}
};

}
}