/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/System/App.h>
#include <MW/Graphics/Graphics.h>
#include <algorithm>

namespace mw {
inline namespace system {

App* App::_instance = nullptr;

sptr<App> App::Get()
{
    if(!_instance)
        throw std::runtime_error("a subclass of App must be instantiated before calling App::Get");
    
    return _instance->This<App>();
}
App::App()
{
    assert(_instance == nullptr);
    _instance = this;

    _dispatcher = ApplicationDispatcher::New();
}

App::~App()
{
    _instance = nullptr;
}

void App::SetMainWindow(sptr<Window> mainWindow)
{
    if (_mainWindow) _mainWindow->RemoveEventHandler(This<IWindowEventHandler>());
    _mainWindow = mainWindow;
    if (_mainWindow) _mainWindow->AddEventHandler(This<IWindowEventHandler>());
}

sptr<ApplicationDispatcher> App::GetDispatcher()
{
    return _dispatcher;
}

sptr<Window> App::GetMainWindow()
{
    return _mainWindow;
}

sptr<Window> App::CreateWindow(
    const std::string title, const IVec2& pos, const IVec2& size)
{
#if PLATFORM_IOS || PLATFORM_ANDROID
    throw std::runtime_error("Creating windows is not supported on this platform. Please use `App::GetMainWindow`.");
#endif

    auto window = Window::New(title, pos, size);
    FlushWindowList();
    _allWindows.push_back(window);
    return window;
}

const std::vector<wptr<Window>>& App::GetWindows()
{
    FlushWindowList();
    return _allWindows;
}

void App::GetWindows(std::vector<sptr<Window>>& windows)
{
    windows.clear();
    windows.reserve(_allWindows.size());

    for (auto wp : _allWindows)
    {
        auto sp = wp.lock();
        if (sp) windows.push_back(sp);
    }
}

void App::SetGraphics(const sptr<GraphicsContext>& graphics) {
    _graphics = graphics;
}

sptr<GraphicsContext> App::GetGraphics() {
    return _graphics;
}

void App::SetAssetLibrary(const sptr<AssetLibrary>& assetLibrary) {
    _assetLibrary = assetLibrary;
}

sptr<AssetLibrary> App::GetAssetLibrary() {
    return _assetLibrary;
}

void App::FlushWindowList() {
    (void)std::remove_if(_allWindows.begin(), _allWindows.end(), [](const wptr<Window>& wp){ return !wp.lock(); });
}

int App::Run(int argc, char *argv[])
{
    auto app = App::Get();
    
    auto dispatcher = app->GetDispatcher();
    Dispatcher::SetCurrent(dispatcher);

    sptr<AppConfig> config = spnew<AppConfig>();

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
