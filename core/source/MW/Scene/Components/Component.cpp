/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/Scene/Components/Component.h>
#include <MW/Scene/Node.h>
#include <MW/Scene/Scene.h>

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

}
}
