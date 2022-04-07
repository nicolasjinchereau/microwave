/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/Scene/Components/Button.h>

namespace mw {
inline namespace scene {

Button::Button()
{
}

void Button::ToJson(json& obj) const
{
    ImageView::ToJson(obj);
}

void Button::FromJson(const json& obj, ObjectLinker* linker)
{
    ImageView::FromJson(obj, linker);
}

void Button::SetAction(const std::function<void()>& act) {
    action = act;
}

void Button::OnPointerDown(Vec2 pos, int id)
{
    if (clickID == -1 && HitTest(pos))
    {
        clickID = id;
        inside = true;

        SetColor(Color(0.5f, 0.5f, 0.5f, 1));
    }
}

void Button::OnPointerMove(Vec2 pos, int id)
{
    if (clickID == id)
    {
        inside = HitTest(pos);

        float shade = inside ? 0.5f : 1;
        SetColor({ shade, shade, shade, 1});
    }
}

void Button::OnPointerUp(Vec2 pos, int id)
{
    if(clickID == id)
    {
        inside = false;
        clickID = -1;

        SetColor(Color::White());

        if(action)
            action();
    }
}

void Button::OnDisable()
{
    if(clickID != -1)
    {
        inside = false;
        clickID = -1;
        SetColor(Color::White());
    }
}

bool Button::HitTest(const Vec2& pointerPos) const
{
    auto hsz = GetSize() * 0.5f;

    auto node = GetNode();
    auto localPos = node->GetLocalPosition();
    auto localPointerPos = node->WorldToLocalPos(pointerPos);

    float left = localPos.x - hsz.x;
    float top = localPos.y + hsz.y;
    float right = localPos.x + hsz.x;
    float bottom = localPos.y - hsz.y;

    return 
        localPointerPos.x >= left &&
        localPointerPos.x <= right &&
        localPointerPos.y >= bottom &&
        localPointerPos.y <= top;
}

} // scene
} // mw
