/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.System.Window;
import Microwave.Graphics.RenderTarget;
import Microwave.Math;
import Microwave.System.Dispatcher;
import Microwave.System.EventHandlerList;
import Microwave.System.Object;
import Microwave.System.Pointers;
import <cstdint>;
import <string>;
import <vector>;

export namespace mw {

inline namespace gfx {
class HWSurface;
}

inline namespace system {

class Window;

enum class Keycode : int
{
    A, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    Num0, Num1, Num2, Num3, Num4,
    Num5, Num6, Num7, Num8, Num9,
    F1, F2, F3, F4, F5, F6,
    F7, F8, F9, F10, F11, F12,
    Back,
    Forward,
    VolumeUp,
    VolumeDown,
    VolumeMute,
    Space,
    Backspace,
    Del,
    LeftArrow,
    RightArrow,
    UpArrow,
    DownArrow,
    Alt,
    Ctrl,
    Shift,
    Enter,
    Escape,
    Equals,
    Minus,
    LeftBracket,
    RightBracket,
    Quote,
    Semicolon,
    Backslash,
    Comma,
    Slash,
    Period,
    Grave,
    Tab,

    // OS specific keycodes not included above will be returned from TranslateKey as (Keycode::Unknown + keycode)
    Unknown
};

class IWindowEventHandler
{
public:
    virtual ~IWindowEventHandler() = default;
    virtual void OnCreate(Window* window) {}
    virtual void OnShow(Window* window) {}
    virtual void OnGotFocus(Window* window) {}
    virtual void OnLostFocus(Window* window) {}
    virtual void OnHide(Window* window) {}
    virtual void OnDestroy(Window* window) {}
    virtual void OnClose(Window* window) {} // window close button pressed
    virtual void OnMove(Window* window, IVec2 pos) {}
    virtual void OnResize(Window* window, IVec2 size) {}

    virtual void OnKeyDown(Window* window, Keycode key) {}
    virtual void OnKeyUp(Window* window, Keycode key) {}
    virtual void OnPointerDown(Window* window, IVec2 pos, int id) {}
    virtual void OnPointerMove(Window* window, IVec2 pos, int id) {}
    virtual void OnPointerUp(Window* window, IVec2 pos, int id) {}
};

class Window : public RenderTarget
{
protected:
    static sptr<Window> New(const std::string title, const IVec2& pos, const IVec2& size);
    friend class App;

    EventHandlerList<IWindowEventHandler> eventHandlers;

    mutable sptr<HWSurface> surface;
public:

    virtual ~Window(){};
    
    virtual void SetTitle(const std::string& title) = 0;
    virtual std::string GetTitle() const = 0;
    virtual void SetPos(const IVec2& pos) = 0;
    virtual IVec2 GetPos() const = 0;
    virtual void SetSize(const IVec2& size) = 0;
    //virtual IVec2 GetSize() const = 0;
    virtual bool IsVisible() const = 0;
    virtual void SetResizeable(bool resizeable) = 0;
    virtual bool IsResizeable() const = 0;

    virtual void Show() = 0;
    virtual void Hide() = 0;
    virtual void Close() = 0;
    virtual uintptr_t GetHandle() const = 0;

    virtual void AddEventHandler(const sptr<IWindowEventHandler>& handler);
    virtual void RemoveEventHandler(const sptr<IWindowEventHandler>& handler);

    virtual void OnCreate();
    virtual void OnShow();
    virtual void OnGotFocus();
    virtual void OnLostFocus();
    virtual void OnHide();
    virtual void OnDestroy();
    virtual void OnClose();
    virtual void OnMove(IVec2 pos);
    virtual void OnResize(IVec2 size);
    virtual void OnKeyDown(Keycode key);
    virtual void OnKeyUp(Keycode key);
    virtual void OnPointerDown(IVec2 pos, int id);
    virtual void OnPointerMove(IVec2 pos, int id);
    virtual void OnPointerUp(IVec2 pos, int id);
};

} // system
} // mw
