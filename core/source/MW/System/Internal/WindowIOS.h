/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module;
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

export module Microwave.System.Internal.WindowIOS;
import Microwave.System.Dispatcher;
import Microwave.System.Pointers;
import Microwave.System.Window;
import Microwave.Math;
import <string>;

@class ViewController;
@class MainWindow;
@class MainView;

export namespace mw {
inline namespace mw {

class WindowIOS : public Window
{
public:
    sptr<Dispatcher> dispatcher;
    MainWindow* mainWindow = nil;
    int buttonsDown = 0;
    bool shiftPressed = false;
    IVec2 pos = IVec2(-1, -1);
    IVec2 size = IVec2(800, 600);
    std::string title;
    bool visible = false;
    bool resizeable = false;
    sptr<HWSurface> surface;

    WindowIOS();
    ~WindowIOS();

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

    MainWindow* CreateNativeWindow();
    MainView* CreateNativeView();

    static Keycode TranslateKey(int keycode);
};

} // system
} // mw

@interface MainWindow : UIWindow
{
    @public ViewController* viewController;
    @public CAEAGLLayer* eaglLayer;
    @public mw::wptr<mw::WindowIOS> window;
    @public UIInterfaceOrientation orientation;
}
@end

