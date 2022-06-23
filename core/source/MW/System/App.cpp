/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module;
#include <MW/System/Internal/Platform.h>

module Microwave.System.App;
import Microwave.Graphics;
import Microwave.System.Dispatcher;
import Microwave.System.Exception;
import Microwave.System.GC;
import Microwave.System.ThreadPool;
import <MW/System/Debug.h>;
import <algorithm>;
import <stdexcept>;

namespace mw {
inline namespace system {

App* App::_instance = nullptr;

gptr<App> App::Get()
{
    if(!_instance)
        throw Exception("a subclass of App must be instantiated before calling App::Get");
    
    return _instance->self(_instance);
}
App::App()
{
    Assert(_instance == nullptr);
    _instance = this;

    _dispatcher = ApplicationDispatcher::New();
}

App::~App()
{
    _instance = nullptr;
}

void App::SetMainWindow(gptr<Window> mainWindow)
{
    if (_mainWindow) _mainWindow->RemoveEventHandler(self(this));
    _mainWindow = mainWindow;
    if (_mainWindow) _mainWindow->AddEventHandler(self(this));
}

gptr<ApplicationDispatcher> App::GetDispatcher()
{
    return _dispatcher;
}

gptr<Window> App::GetMainWindow()
{
    return _mainWindow;
}

gptr<Window> App::CreateWindow(
    const std::string title, const IVec2& pos, const IVec2& size)
{
#if PLATFORM_IOS || PLATFORM_ANDROID
    throw Exception("Creating windows is not supported on this platform. Please use `App::GetMainWindow`.");
#endif

    auto window = Window::New(title, pos, size);
    FlushWindowList();
    _allWindows.push_back(window);
    return window;
}

const gvector<wgptr<Window>>& App::GetWindows()
{
    FlushWindowList();
    return _allWindows;
}

void App::GetWindows(gvector<gptr<Window>>& windows)
{
    windows.clear();
    windows.reserve(_allWindows.size());

    for (auto wp : _allWindows)
    {
        auto sp = wp.lock();
        if (sp) windows.push_back(sp);
    }
}

void App::SetAssetLibrary(const gptr<AssetLibrary>& assetLibrary) {
    _assetLibrary = assetLibrary;
}

gptr<AssetLibrary> App::GetAssetLibrary() {
    return _assetLibrary;
}

void App::FlushWindowList() {
    (void)std::remove_if(_allWindows.begin(), _allWindows.end(),
        [](const wgptr<Window>& wp){ return !wp.lock(); });
}

Task<void> DoCollection()
{
    co_await GC::CollectAsync();

    auto next = std::chrono::steady_clock::now() + std::chrono::milliseconds(2000);
    Dispatcher::GetCurrent()->InvokeAsync([]{ DoCollection(); }, next);
}

int App::Run(int argc, char *argv[])
{
    auto app = App::Get();
    
    auto dispatcher = app->GetDispatcher();
    Dispatcher::SetCurrent(dispatcher);

    gptr<AppConfig> config = gpnew<AppConfig>();

    DoCollection();
    
    dispatcher->InvokeAsync([app, config]{
        app->OnInitialize(*config);
    });

    dispatcher->InvokeAsync([app, config]{
        auto window = Window::New(config->windowTitle, config->windowPos, config->windowSize);
        app->_allWindows.push_back(window);
        app->SetMainWindow(window);
        window->Show();
        app->OnStart();
    });

    dispatcher->Run(argc, argv);
    app->OnQuit();
    
    return 0;
}

} // system
} // mw
