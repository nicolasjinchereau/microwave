/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Scene/Events.h>
#include <MW/Scene/Components/View.h>
#include <MW/Scene/Components/ImageView.h>
#include <functional>

namespace mw {
inline namespace scene {

class Button : public ImageView
             , public ICanvasInputEvents
{
    bool down = false;
    bool inside = false;
    int clickID = -1;
    std::function<void()> action;
public:

    Button();

    void SetAction(const std::function<void()>& act);

    virtual void ToJson(json& obj) const override;
    virtual void FromJson(const json& obj, ObjectLinker* linker) override;

    virtual void OnPointerDown(Vec2 pos, int id) override;
    virtual void OnPointerMove(Vec2 pos, int id) override;
    virtual void OnPointerUp(Vec2 pos, int id) override;

    virtual void OnDisable() override;
private:
    bool HitTest(const Vec2& pointerPos) const;
};

} // scene
} // mw
