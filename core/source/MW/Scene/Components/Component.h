/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/System/Object.h>
#include <MW/Scene/Events.h>
#include <MW/System/Clock.h>

namespace mw {
inline namespace scene {

class Node;
class Scene;
class Camera;

class Component : public virtual EventTarget, public Object
{
    REGISTER_OBJECT(Component);
    friend class Node;
public:

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
