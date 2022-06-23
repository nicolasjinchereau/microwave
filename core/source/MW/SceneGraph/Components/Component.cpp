/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.SceneGraph.Components.Component;
import Microwave.SceneGraph.Node;
import Microwave.SceneGraph.Scene;

namespace mw {
inline namespace scene {

gptr<Clock> Component::GetClock() {
    return GetNode()->GetScene()->GetClock();
}

gptr<const Clock> Component::GetClock() const {
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

bool Component::IsEnabled() const {
    return enabled;
}

void Component::SetEnabled(bool enable)
{
    if (enable != enabled)
    {
        enabled = enable;
        if (enabled)
            OnEnable();
        else
            OnDisable();
    }
}

bool Component::IsNodeBranchActive() const
{
    auto n = node.lock();
    return n ? n->IsBranchActive() : false;
}

gptr<const Node> Component::GetNode() const {
    return node.lock();
}

gptr<Node> Component::GetNode() {
    return node.lock();
}

gptr<const Scene> Component::GetScene() const {
    auto n = node.lock();
    return n ? n->GetScene() : nullptr;
}

gptr<Scene> Component::GetScene() {
    auto n = node.lock();
    return n ? n->GetScene() : nullptr;
}

bool Component::IsActiveAndEnabled() const
{
    gptr<Node> n;
    return enabled && ((n = node.lock()) ? n->IsBranchActive() : false);
}

} // scene
} // mw
