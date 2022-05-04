/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.System.Internal.WindowAndroid;
import Microwave.Graphics.Internal.HWSurface;
import Microwave.System.Dispatcher;
import Microwave.System.Pointers;
import Microwave.System.Window;
import Microwave.Math;
import <android_native_app_glue.h>;
import <android/native_window.h>;
import <android/native_activity.h>;
import <string>;

export namespace mw {
inline namespace system {

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
    sptr<HWSurface> surface;

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

    virtual sptr<HWRenderTarget> GetHWRenderTarget() override;

    static Keycode TranslateKey(int keycode);
    void SetSizeInternal(const IVec2& size);
};

} // system
} // mw
