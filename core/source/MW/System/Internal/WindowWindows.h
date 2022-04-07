/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/System/Window.h>
#include <MW/System/Pointers.h>
#include <MW/Math/Math.h>
#include <Windows.h>
#include <string>

namespace mw {
inline namespace system {

class WindowWindows : public Window
{
public:
    sptr<Dispatcher> dispatcher;
    HWND hWnd = nullptr;
    HDC hDC = nullptr;
    int buttonsDown = 0;
    IVec2 pos = IVec2(-1, -1);
    IVec2 size = IVec2(800, 600);
    std::string title;
    bool visible = false;
    bool resizeable = false;
    sptr<WindowSurface> surface;

    WindowWindows();
    WindowWindows(const std::string title, const IVec2& pos, const IVec2& size);
    ~WindowWindows();

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

    HWND CreateNativeWindow(const std::string title, const IVec2& pos, const IVec2& size, bool resizeable);

    static LRESULT CALLBACK CreateWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static Keycode TranslateKey(int keycode);

    static int GetWindowHeight(HWND hWnd);
    static std::string GetWindowTitle(HWND hWnd);

    virtual sptr<WindowSurface> GetSurface() override;
};

} // system
} // mw