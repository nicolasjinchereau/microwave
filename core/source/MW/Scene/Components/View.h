/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Graphics/Types.h>
#include <MW/Math/Math.h>
#include <MW/Scene/Components/Component.h>
#include <MW/Scene/Events.h>
#include <MW/Scene/Node.h>

namespace mw {
inline namespace scene {

class Canvas;

class View : public Component, public ISystemEvents
{
    REGISTER_OBJECT(View);
protected:
    Vec2 size;
    bool anchorEnabled = true;
    Box anchorBox{ 0, 1, 1, 0 };
    Box offsetBox{ 0, 0, 0, 0 };
    int renderDepth = 0;

    friend class Canvas;
public:
    virtual void ToJson(json& obj) const override;
    virtual void FromJson(const json& obj, ObjectLinker* linker) override;

    void SetRenderDepth(int depth) { renderDepth = depth; }
    int GetRenderDepth() const { return renderDepth; }

    void SetSize(const Vec2& newSize);
    Vec2 GetSize() const;

    void SetAnchorEnabled(bool enabled);
    bool GetAnchorEnabled() const;

    void SetAnchor(const Box& box);
    Box GetAnchor() const;

    void SetAnchorOffset(const Box& box);
    Box GetAnchorOffset() const;

    virtual void OnSizeChanged() {}
    virtual void SystemLateUpdate() override;
};

} // scene
} // mw
