/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.SceneGraph.Events;
import Microwave.SceneGraph.Node;
import Microwave.SceneGraph.Scene;

namespace mw {
inline namespace scene {

bool EventTarget::HasEventBit(uint32_t bit) const {
    return eventBits & bit;
}

bool EventTarget::IsEnabled() const {
    return enabled;
}

void EventTarget::SetEnabled(bool enable)
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

bool EventTarget::IsNodeBranchActive() const
{
    auto n = node.lock();
    return n ? n->IsBranchActive() : false;
}

sptr<const Node> EventTarget::GetNode() const {
    return node.lock();
}

sptr<Node> EventTarget::GetNode() {
    return node.lock();
}

sptr<const Scene> EventTarget::GetScene() const {
    auto n = node.lock();
    return n ? n->GetScene() : nullptr;
}

sptr<Scene> EventTarget::GetScene() {
    auto n = node.lock();
    return n ? n->GetScene() : nullptr;
}

bool EventTarget::IsActiveAndEnabled() const
{
    sptr<Node> n;
    return enabled && ((n = node.lock()) ? n->IsBranchActive() : false);
}

} // scene
} // mw
