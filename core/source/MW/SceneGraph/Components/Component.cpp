/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.SceneGraph.Components.Component;
import Microwave.SceneGraph.Node;
import Microwave.SceneGraph.Scene;

namespace mw {
inline namespace scene {

sptr<Clock> Component::GetClock() {
    return GetNode()->GetScene()->GetClock();
}

sptr<const Clock> Component::GetClock() const {
    return GetNode()->GetScene()->GetClock();
}

void Component::ToJson(json& obj) const
{
    Object::ToJson(obj);
    obj["enabled"] = enabled;
}

void Component::FromJson(const json& obj, ObjectLinker* linker)
{
    Object::FromJson(obj, linker);
    enabled = obj.value("enabled", enabled);
}

} // scene
} // mw
