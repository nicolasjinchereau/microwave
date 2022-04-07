/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Math/Math.h>
#include <MW/Scene/Components/Camera.h>
#include <MW/Scene/Components/Component.h>
#include <MW/Scene/Components/View.h>
#include <MW/Scene/Events.h>
#include <MW/System/Window.h>

namespace mw {
inline namespace scene {

class Camera;

enum class FitMode
{
    // just use reference size
    None,

    // adjust width to match render target aspect
    AdjustWidth,

    // adjust height to match render target aspect
    AdjustHeight,

    // adjust width and height to match target size
    AdjustBoth
};

class Canvas : public View
{
    REGISTER_OBJECT(Canvas);
protected:
    std::vector<sptr<Node>> row0;
    std::vector<sptr<Node>> row1;
    std::vector<sptr<ICanvasInputEvents>> inputHandlers;
    
    Vec2 referenceSize;
    FitMode fitMode = FitMode::AdjustWidth;
    wptr<Camera> camera;
    
    bool structureDirty = true;

    void FitCanvasToTarget();
    void FitCameraToCanvas();
public:

    virtual void ToJson(json& obj) const override;
    virtual void FromJson(const json& obj, ObjectLinker* linker) override;

    Canvas();

    void SetReferenceSize(const Vec2& size);
    Vec2 GetReferenceSize() const;

    void SetFitMode(FitMode mode);
    FitMode GetFitMode() const;

    void SetCamera(const sptr<Camera>& camera);
    sptr<Camera> GetCamera() const;

    Vec2 WorldToCanvasPos(const Vec3& pos, const sptr<Camera>& camera) const;
    Vec2 WindowToCanvasPos(Window* window, IVec2 pos) const;

    void SendKeyDown(Window* window, Keycode key);
    void SendKeyUp(Window* window, Keycode key);
    void SendPointerDown(Window* window, IVec2 pos, int id);
    void SendPointerMove(Window* window, IVec2 pos, int id);
    void SendPointerUp(Window* window, IVec2 pos, int id);

    void UpdateStructure();
    void UpdateInputHandlers();

    virtual void OnStructureChanged() override;
    virtual void OnAttachedToScene() override;

    virtual void SystemLateUpdate() override;

};

inline void to_json(json& obj, const FitMode& fm)
{
    static std::unordered_map<FitMode, std::string> names {
        { FitMode::None, "None" },
        { FitMode::AdjustWidth, "AdjustWidth" },
        { FitMode::AdjustHeight, "AdjustHeight" },
        { FitMode::AdjustBoth, "AdjustBoth" }
    };
    obj = names[fm];
}

inline void from_json(const json& obj, FitMode& fm)
{
    static std::unordered_map<std::string, FitMode> values {
        { "None", FitMode::None },
        { "AdjustWidth", FitMode::AdjustWidth },
        { "AdjustHeight", FitMode::AdjustHeight },
        { "AdjustBoth", FitMode::AdjustBoth }
    };
    fm = values[obj.get<std::string>()];
}

} // scene
} // mw
