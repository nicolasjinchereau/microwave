/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/Scene/Node.h>
#include <MW/Scene/Components/Collider.h>

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
    ObjectLinker::RestoreLink(linker, This(), pivot, obj, "pivot");
}

void Collider::SetPivot(const sptr<Node>& pv) {
    pivot = pv;
}

sptr<Node> Collider::GetPivot()
{
    auto p = pivot.lock();
    if (!p) pivot = (p = GetNode());
    return p;
}

} // scene
} // mw
