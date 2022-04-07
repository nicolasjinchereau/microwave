/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <string>
#include <MW/System/Window.h>
#include <MW/System/Pointers.h>
#include <MW/Math/Math.h>
#include <android_native_app_glue.h>
#include <android/native_window.h>
#include <android/native_activity.h>

namespace mw {

class WindowAndroid : public Window
{
    static WindowAndroid* instance;
    static int32_t OnInput(struct android_app* app, AInputEvent* event);

public:
    sptr<Dispatcher> dispatcher;
    //MainWindow* mainWindow = nil;
    int buttonsDown = 0;
    bool shiftPressed = false;
    IVec2 pos = IVec2(-1, -1);
    IVec2 size = IVec2(800, 600);
    std::string title;
    bool visible = false;
    bool resizeable = false;
    sptr<gfx::WindowSurface> surface;

    WindowAndroid();
    ~WindowAndroid();

    virtual void SetTitle(const std::string& title) override;
    virtual std::string GetTitle() const override;
    virtual void SetPos(const IVec2& pos) override;
    virtual IVec2 GetPos() const override;
    virtual void SetSize(const IVec2& size) override;
    virtual IVec2 GetSize() const override;
    virtual bool IsVisible() const override;
    virtual void SetResizeable(bool resizeable) override;
    virtual bool IsResizeable() const override;

    virtual void Show() override;
    virtual void Hide() override;
    virtual void Close()  override;
    virtual uintptr_t GetHandle() const override;

    static Keycode TranslateKey(int keycode);

    virtual sptr<gfx::WindowSurface> GetSurface() override;

    void SetSizeInternal(const IVec2& size);
};

}
