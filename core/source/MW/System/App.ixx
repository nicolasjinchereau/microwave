/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.System.App;
import Microwave.Audio.AudioContext;
import Microwave.Data.Library.AssetLibrary;
import Microwave.Graphics.GraphicsContext;
import Microwave.Math;
import Microwave.System.ApplicationDispatcher;
import Microwave.System.Pointers;
import Microwave.System.Object;
import Microwave.System.Spinlock;
import Microwave.System.Window;
import <cstdint>;
import <mutex>;
import <string>;
import <vector>;

export namespace mw {
inline namespace system {

class AppConfig
{
public:
    std::string windowTitle;
    IVec2 windowPos;
    IVec2 windowSize;
};

class App : public Object, public IWindowEventHandler
{
    static App* _instance;
    gptr<ApplicationDispatcher> _dispatcher;
protected:
    gptr<Window> _mainWindow;
    gvector<wgptr<Window>> _allWindows;
    gptr<AssetLibrary> _assetLibrary;

    void SetMainWindow(gptr<Window> mainWindow);

    void FlushWindowList();
public:
    App();
    ~App();

    static gptr<App> Get();
    gptr<ApplicationDispatcher> GetDispatcher();
    
    // only works on desktop platforms
    gptr<Window> CreateWindow(const std::string title, const IVec2& pos, const IVec2& size);
    const gvector<wgptr<Window>>& GetWindows();
    void GetWindows(gvector<gptr<Window>>& windows);
    gptr<Window> GetMainWindow();

    void SetAssetLibrary(const gptr<AssetLibrary>& assetLibrary);
    gptr<AssetLibrary> GetAssetLibrary();

    virtual void OnInitialize(AppConfig& config) {}
    virtual void OnStart() {}
    virtual void OnActivate() {}
    virtual void OnDeactivate() {}
    virtual void OnQuit() {}
    
    // events from App's main window
    virtual void OnCreate(Window* window) override {}
    virtual void OnShow(Window* window) override {}
    virtual void OnGotFocus(Window* window) override {}
    virtual void OnLostFocus(Window* window) override {}
    virtual void OnHide(Window* window) override {}
    virtual void OnDestroy(Window* window) override {}
    virtual void OnClose(Window* window) override {} // window close button pressed
    virtual void OnMove(Window* window, IVec2 pos) override {}
    virtual void OnResize(Window* window, IVec2 size) override {}
    virtual void OnKeyDown(Window* window, Keycode key) override {}
    virtual void OnKeyUp(Window* window, Keycode key) override {}
    virtual void OnPointerDown(Window* window, IVec2 pos, int id) override {}
    virtual void OnPointerMove(Window* window, IVec2 pos, int id) override {}
    virtual void OnPointerUp(Window* window, IVec2 pos, int id) override {}
    //*********************

    static int Run(int argc, char *argv[]);
};

} // system
} // mw
