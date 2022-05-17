/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.SceneGraph.Components.Canvas;
import Microwave.Graphics.GraphicsContext;
import Microwave.Graphics.RenderTarget;
import Microwave.SceneGraph.Node;
import Microwave.System.Object;
import <cassert>;
import <string>;
import <unordered_map>;
import <vector>;

namespace mw {
inline namespace scene {

void Canvas::ToJson(json& obj) const
{
    View::ToJson(obj);

    obj["referenceSize"] = referenceSize;
    obj["fitMode"] = fitMode;

    ObjectLinker::SaveLink(obj, "camera", camera.lock());
}

void Canvas::FromJson(const json& obj, ObjectLinker* linker)
{
    View::FromJson(obj, linker);

    referenceSize = obj.value("referenceSize", referenceSize);
    fitMode = obj.value("fitMode", fitMode);

    ObjectLinker::RestoreLink(linker, SharedFrom(this), camera, obj, "camera");

    structureDirty = true;
}

Canvas::Canvas() {}

void Canvas::SetReferenceSize(const Vec2& size) {
    referenceSize = size;
}

Vec2 Canvas::GetReferenceSize() const {
    return referenceSize;
}

void Canvas::SetFitMode(FitMode mode) {
    fitMode = mode;
}

FitMode Canvas::GetFitMode() const {
    return fitMode;
}

void Canvas::SetCamera(const sptr<Camera>& camera) {
    this->camera = camera;
}

sptr<Camera> Canvas::GetCamera() const {
    return camera.lock();
}

void Canvas::OnStructureChanged() {
    structureDirty = true;
}

void Canvas::OnAttachedToScene() {
    structureDirty = true;
}

void Canvas::UpdateStructure()
{
    if(structureDirty)
    {
        struct R
        {
            static void SetViewDepths(
                const sptr<Node>& canvasNode,
                const sptr<Node>& node,
                int depth)
            {
                for (auto& c : node->GetComponents())
                {
                    if (depth != 0 && spcast<Canvas>(c))
                        return;

                    if (auto v = spcast<View>(c)) {
                        v->SetRenderDepth(depth);
                    }
                }

                for (auto& n : node->GetChildren())
                    SetViewDepths(canvasNode, n, depth + 1);
            }
        };

        auto canvasNode = GetNode();
        R::SetViewDepths(canvasNode, canvasNode, 0);

        structureDirty = false;
    }
}

void Canvas::UpdateInputHandlers()
{
    inputHandlers.clear();
    row0.push_back(GetNode());

    while (!row0.empty())
    {
        for (auto it = row0.rbegin(); it != row0.rend(); ++it)
        {
            auto& n = *it;

            if (auto h = n->GetComponent<ICanvasInputEvents>())
                inputHandlers.push_back(h);

            for (auto& child : n->GetChildren())
            {
                if (child->IsBranchActive() && !child->GetComponent<Canvas>())
                    row1.push_back(child);
            }
        }

        row0.swap(row1);
        row1.clear();
    }

    row0.clear();
    row1.clear();
    std::reverse(inputHandlers.begin(), inputHandlers.end());
}

void Canvas::SystemLateUpdate()
{
    View::SystemLateUpdate();
    
    UpdateStructure();
    UpdateInputHandlers();

    FitCanvasToTarget();
    FitCameraToCanvas();
}

Vec2 Canvas::WorldToCanvasPos(const Vec3& pos, const sptr<Camera>& camera) const
{
    assert(camera);

    auto graphics = GraphicsContext::GetCurrent();
    assert(graphics);

    auto screenPos = camera->WorldToScreen(pos);
    auto viewport = graphics->GetViewport();
    auto xn = (screenPos.x - viewport.x) / (float)viewport.w;
    auto yn = (screenPos.y - viewport.y) / (float)viewport.h;

    auto canvasSize = GetSize();
    auto x = (xn - 0.5f) * canvasSize.x;
    auto y = (yn - 0.5f) * canvasSize.y;
    
    return Vec2(x, y);
}

Vec2 Canvas::WindowToCanvasPos(Window* window, IVec2 pos) const
{
    auto canvasSize = GetSize();
    auto windowSize = window->GetSize();
    auto xn = (pos.x + 0.5f) / (float)windowSize.x;
    auto yn = (pos.y + 0.5f) / (float)windowSize.y;
    auto x = (xn - 0.5f) * canvasSize.x;
    auto y = ((1.0f - yn) - 0.5f) * canvasSize.y;
    return Vec2(x, 1.0f - y);
}

void Canvas::SendKeyDown(Window* window, Keycode key)
{
    for (auto& h : inputHandlers)
        h->OnKeyDown(key);
}

void Canvas::SendKeyUp(Window* window, Keycode key)
{
    for (auto& h : inputHandlers)
        h->OnKeyUp(key);
}

void Canvas::SendPointerDown(Window* window, IVec2 pos, int id)
{
    auto canvasPos = WindowToCanvasPos(window, pos);

    for (auto& h : inputHandlers)
        h->OnPointerDown(canvasPos, id);
}

void Canvas::SendPointerMove(Window* window, IVec2 pos, int id)
{
    auto canvasPos = WindowToCanvasPos(window, pos);

    for (auto& h : inputHandlers)
        h->OnPointerMove(canvasPos, id);
}

void Canvas::SendPointerUp(Window* window, IVec2 pos, int id)
{
    auto canvasPos = WindowToCanvasPos(window, pos);

    for (auto& h : inputHandlers)
        h->OnPointerUp(canvasPos, id);
}

void Canvas::FitCanvasToTarget()
{
    auto graphics = GraphicsContext::GetCurrent();

    if (auto target = graphics->GetRenderTarget())
    {
        if (fitMode == FitMode::AdjustWidth)
        {
            auto targetSize = target->GetSize();
            auto aspect = (float)targetSize.x / targetSize.y;
            SetSize(Vec2(referenceSize.y * aspect, referenceSize.y));
        }
        else if (fitMode == FitMode::AdjustHeight) {
            auto targetSize = target->GetSize();
            auto aspect = (float)targetSize.y / targetSize.x;
            SetSize(Vec2(referenceSize.x, referenceSize.x * aspect));
        }
        else if (fitMode == FitMode::AdjustBoth) {
            auto targetSize = target->GetSize();
            SetSize(Vec2((float)targetSize.x, (float)targetSize.y));
        }
        else {
            SetSize(referenceSize);
        }
    }
}

void Canvas::FitCameraToCanvas()
{
    if (auto cam = camera.lock())
    {
        auto node = cam->GetNode();
        node->SetPosition(Vec3(0, 0, -1.0f));
        node->SetRotation(Quat::Identity());
        node->SetScale(Vec3::One());

        cam->SetMode(CameraViewMode::OrthoFixedHeight);
        cam->SetFixedHeight(size.y);
        cam->SetNearPlane(0.0001f);
        cam->SetFarPlane(2.0f);
    }
}

} // scene
} // mw
