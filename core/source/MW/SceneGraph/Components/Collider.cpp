/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.SceneGraph.Components.Collider;
import Microwave.SceneGraph.Node;

namespace mw {
inline namespace scene {

void Collider::ToJson(json& obj) const
{
    Component::ToJson(obj);
    ObjectLinker::SaveLink(obj, "pivot", pivot.lock());
}

void Collider::FromJson(const json& obj, ObjectLinker* linker)
{
    Component::FromJson(obj, linker);
    ObjectLinker::RestoreLink(linker, self(this), pivot, obj, "pivot");
}

void Collider::SetPivot(const gptr<Node>& pv) {
    pivot = pv;
}

gptr<Node> Collider::GetPivot()
{
    auto p = pivot.lock();
    if (!p) pivot = (p = GetNode());
    return p;
}

} // scene
} // mw
