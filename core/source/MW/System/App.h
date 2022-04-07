/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Audio/AudioContext.h>
#include <MW/Data/Library/AssetLibrary.h>
#include <MW/Graphics/GraphicsContext.h>
#include <MW/Math/Math.h>
#include <MW/System/ApplicationDispatcher.h>
#include <MW/System/Clock.h>
#include <MW/System/Pointers.h>
#include <MW/System/Spinlock.h>
#include <MW/System/Window.h>
#include <mutex>
#include <vector>
#include <cstdint>

namespace mw {
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
    sptr<ApplicationDispatcher> _dispatcher;
protected:
    sptr<Window> _mainWindow;
    std::vector<wptr<Window>> _allWindows;
    sptr<GraphicsContext> _graphics;
    sptr<AssetLibrary> _assetLibrary;

    void SetMainWindow(sptr<Window> mainWindow);

    void FlushWindowList();
public:
    App();
    ~App();

    static sptr<App> Get();
    sptr<ApplicationDispatcher> GetDispatcher();
    
    // only works on desktop platforms
    sptr<Window> CreateWindow(const std::string title, const IVec2& pos, const IVec2& size);
    const std::vector<wptr<Window>>& GetWindows();
    void GetWindows(std::vector<sptr<Window>>& windows);
    sptr<Window> GetMainWindow();

    void SetGraphics(const sptr<GraphicsContext>& graphics);
    sptr<GraphicsContext> GetGraphics();

    void SetAssetLibrary(const sptr<AssetLibrary>& assetLibrary);
    sptr<AssetLibrary> GetAssetLibrary();

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
