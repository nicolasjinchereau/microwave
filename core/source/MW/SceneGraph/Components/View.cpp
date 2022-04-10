/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.SceneGraph.Components.View;
import Microwave.SceneGraph.Node;

namespace mw {
inline namespace scene {

void View::ToJson(json& obj) const
{
    Component::ToJson(obj);
    obj["size"] = size;
    obj["anchorEnabled"] = anchorEnabled;
    obj["anchorBox"] = anchorBox;
    obj["offsetBox"] = offsetBox;
    obj["renderDepth"] = renderDepth;
}

void View::FromJson(const json& obj, ObjectLinker* linker)
{
    Component::FromJson(obj, linker);
    size = obj.value("size", size);
    anchorEnabled = obj.value("anchorEnabled", anchorEnabled);
    anchorBox = obj.value("anchorBox", anchorBox);
    offsetBox = obj.value("offsetBox", offsetBox);
    renderDepth = obj.value("renderDepth", renderDepth);
}

void View::SetAnchorEnabled(bool enabled) {
    anchorEnabled = enabled;
}

bool View::GetAnchorEnabled() const {
    return anchorEnabled;
}

void View::SetSize(const Vec2& newSize)
{
    if (newSize != size) {
        size = newSize;
        OnSizeChanged();
    }
}

Vec2 View::GetSize() const {
    return size;
}

void View::SetAnchor(const Box& box) {
    anchorBox = box;
}

Box View::GetAnchor() const {
    return anchorBox;
}

void View::SetAnchorOffset(const Box& box) {
    offsetBox = box;
}

Box View::GetAnchorOffset() const {
    return offsetBox;
}

void View::SystemLateUpdate()
{
    auto node = GetNode();

    auto parentView = node->GetParent()->GetComponent<View>();
    if (parentView && anchorEnabled)
    {
        auto parentSize = parentView->GetSize();

        float lt = parentSize.x * (anchorBox.left - 0.5f) + offsetBox.left;
        float tp = parentSize.y * (anchorBox.top - 0.5f) + offsetBox.top;
        float rt = parentSize.x * (anchorBox.right - 0.5f) + offsetBox.right;
        float bt = parentSize.y * (anchorBox.bottom - 0.5f) + offsetBox.bottom;
        SetSize(Vec2(rt - lt, tp - bt));

        auto localPos = node->GetLocalPosition();
        localPos.x = lt + size.x * 0.5f;
        localPos.y = bt + size.y * 0.5f;
        node->SetLocalPosition(localPos);
    }
}

} // scene
} // mw
