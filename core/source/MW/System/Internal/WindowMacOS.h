/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module;
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import <AppKit/AppKit.h>
#import <Carbon/Carbon.h>

export module Microwave.System.Internal.WindowMacOS;
import Microwave.Math;
import Microwave.System.Dispatcher;
import Microwave.System.Pointers;
import Microwave.System.Window;
import <string>;

@class MainWindow;

export namespace mw {
inline namespace system {

class WindowMacOS : public Window
{
public:
    gptr<Dispatcher> dispatcher;
    MainWindow* mainWindow = nil;
    int buttonsDown = 0;
    bool shiftPressed = false;
    IVec2 pos = IVec2(-1, -1);
    IVec2 size = IVec2(800, 600);
    std::string title;
    bool visible = false;
    bool resizeable = false;
    gptr<HWSurface> surface;

    WindowMacOS();
    WindowMacOS(const std::string title, const IVec2& pos, const IVec2& size);
    ~WindowMacOS();

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

    virtual gptr<HWRenderTarget> GetHWRenderTarget() override;

    MainWindow* CreateNativeWindow(const std::string title, const IVec2& pos, const IVec2& size, bool resizeable);
    
    static Keycode TranslateKey(int keycode);
};

} // system
} // mw

@interface MainWindow : NSWindow<NSWindowDelegate>
{
    @public mw::gwptr<mw::WindowMacOS> window;
}
-(MainWindow*)initWithContentRect:(NSRect)contentRect;
@end
