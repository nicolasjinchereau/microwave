/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module;
#include <MW/System/Internal/PlatformHeaders.h>

export module Microwave.System.Internal.WindowWindows;
import Microwave.Math;
import Microwave.System.Dispatcher;
import Microwave.System.Pointers;
import Microwave.System.Window;
import std;

export namespace mw {

inline namespace gfx {
class HWRenderTarget;
}

inline namespace system {

class WindowWindows : public Window
{
public:
    gptr<Dispatcher> dispatcher;
    HWND hWnd = nullptr;
    HDC hDC = nullptr;
    int buttonsDown = 0;
    WindowConfig config;
    bool visible = false;

    WindowWindows();
    WindowWindows(const WindowConfig& config);
    ~WindowWindows();

    virtual void SetTitle(const std::string& title) override;
    virtual std::string GetTitle() const override;
    virtual void SetPos(const IVec2& pos) override;
    virtual IVec2 GetPos() const override;
    virtual void SetSize(const IVec2& size) override;
    virtual IVec2 GetSize() const override;
    virtual bool IsVisible() const override;
    virtual void SetResizable(bool resizable) override;
    virtual bool IsResizable() const override;
    virtual std::uint32_t GetDPI() const override;

    virtual void Show() override;
    virtual void Hide() override;
    virtual void Close()  override;
    virtual uintptr_t GetHandle() const override;

    HWND CreateNativeWindow(const WindowConfig& config);

    static LRESULT CALLBACK CreateWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static Keycode TranslateKey(int keycode);

    static int GetWindowHeight(HWND hWnd);
    static std::string GetWindowTitle(HWND hWnd);

    virtual gptr<HWRenderTarget> GetHWRenderTarget() override;
};

} // system
} // mw
