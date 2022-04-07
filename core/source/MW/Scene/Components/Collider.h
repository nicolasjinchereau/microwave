/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Scene/Components/Component.h>
#include <MW/Math/Math.h>

namespace mw {
inline namespace scene {

class Collider : public Component
{
protected:
    wptr<Node> pivot;
    bool dirty = false;
public:
    virtual void ToJson(json& obj) const override;
    virtual void FromJson(const json& obj, ObjectLinker* linker) override;

    void SetPivot(const sptr<Node>& pivot);
    sptr<Node> GetPivot();

    void SetDirty(bool dirty) { this->dirty = dirty; }
    bool IsDirty() const { return dirty; }
};

} // scene
} // mw
