/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.System.Internal.ApplicationDispatcherAndroid;
import Microwave.Graphics.GraphicsContext;
import Microwave.Graphics.Internal.HWSurfaceeEGL;
import Microwave.System.Internal.WindowAndroid;
import Microwave.System.Application;
import <android/sensor.h>;
import <android/log.h>;
import <android/asset_manager.h>;
import <android/keycodes.h>;
import <android_native_app_glue.h>;
import <unistd.h>;
import <stdexcept>;
import <functional>;
import <algorithm>;

namespace detail {
    thread_local mw::gptr<mw::ApplicationDispatcherAndroid> _mainThreadDispatcher;
}

namespace mw {
inline namespace system {

extern android_app* androidApp;

gptr<ApplicationDispatcher> ApplicationDispatcher::New() {
    detail::_mainThreadDispatcher = gpnew<ApplicationDispatcherAndroid>();
    return detail::_mainThreadDispatcher;
}

ApplicationDispatcherAndroid::ApplicationDispatcherAndroid()
{

}

ApplicationDispatcherAndroid::~ApplicationDispatcherAndroid()
{
}

gptr<DispatchAction> ApplicationDispatcherAndroid::InvokeAsync(
    std::function<void()> function,
    DispatchTime when
)
{
    std::unique_lock<std::mutex> lk(mut);

    auto action = gpnew<DispatchAction>(std::move(function), when);
    sorted_insert(actions, action, DispatchActionComparison());
    Wake();
    return action;
}

void ApplicationDispatcherAndroid::Wake()
{
    if (run && processEvents) {
        ALooper_wake(looper);
    }
}

void ApplicationDispatcherAndroid::Quit() {
    run = false;
    Wake();
}

long long ApplicationDispatcherAndroid::GetDispatchTimeout()
{
    long long timeout = -1;

    if (run && processEvents)
    {
        auto now = DispatchClock::now();

        if (!actions.empty())
        {
            auto when = actions.front()->when;
            auto to = std::chrono::duration_cast<std::chrono::milliseconds>(when - now).count();
            to = std::max(to, 0LL);
            timeout = to;
        }

        if (continuousDispatchRate > 0)
        {
            auto when = continuousDispatchWakeTime;
            auto to = std::chrono::duration_cast<std::chrono::milliseconds>(when - now).count();
            to = std::max(to, 0LL);
            timeout = (timeout >= 0) ? std::min(to, timeout) : to;
        }
    }

    return timeout;
}

void ApplicationDispatcherAndroid::OnCommand(android_app* pApp, int32_t cmd)
{
    auto dispatcher = (ApplicationDispatcherAndroid*)pApp->userData;

    switch (cmd)
    {
    case APP_CMD_INIT_WINDOW:
        Console::WriteLine("APP_CMD_INIT_WINDOW");
        dispatcher->OnWindowCreated();
        break;

    case APP_CMD_START:
        Console::WriteLine("APP_CMD_START");
        dispatcher->OnWindowShow();
        break;

    case APP_CMD_RESUME:
        Console::WriteLine("APP_CMD_RESUME");
        dispatcher->OnWindowGotFocus();
        break;

    case APP_CMD_PAUSE:
        Console::WriteLine("APP_CMD_PAUSE");
        dispatcher->OnWindowLostFocus();
        break;

    case APP_CMD_STOP:
        Console::WriteLine("APP_CMD_STOP");
        dispatcher->OnWindowHide();
        break;

    case APP_CMD_TERM_WINDOW:
        Console::WriteLine("APP_CMD_TERM_WINDOW");
        dispatcher->OnWindowDestroyed();
        break;

    case APP_CMD_WINDOW_RESIZED:
        Console::WriteLine("APP_CMD_WINDOW_RESIZED");
        dispatcher->OnWindowResized();
        break;

    case APP_CMD_CONFIG_CHANGED:
        Console::WriteLine("APP_CMD_CONFIG_CHANGED");
        dispatcher->OnConfigurationChanged();
        break;
    }
}

void ApplicationDispatcherAndroid::OnWindowCreated()
{
    InvokeAsync([]() {
        auto app = Application::GetInstance();

        // reallocate graphics resources for existing surfaces
        std::vector<gptr<Window>> windows;
        app->GetWindows(windows);

        for (auto& win : windows)
        {
            if (auto surface = win->GetSurface())
                surface->AllocateGraphicsResource();
        }

        // rebind render targets of existing graphics contexts
        std::vector<gptr<gfx::GraphicsContext>> graphicsContexts;
        app->GetGraphicsContexts(graphicsContexts);

        for (auto& gfx : graphicsContexts)
        {
            gfx->RebindRenderTarget();
        }

        // dispatch OnCreate event for main window
        int width = ANativeWindow_getWidth(androidApp->window);
        int height = ANativeWindow_getHeight(androidApp->window);
        Console::WriteLine("WINDOW CREATED: (% x %)", width, height);

        auto mainWindow = std::dynamic_pointer_cast<WindowAndroid>(app->GetMainWindow());
        if (mainWindow)
            mainWindow->OnCreate();
        });

    processEvents = true;
    Wake();
}

void ApplicationDispatcherAndroid::OnWindowShow()
{
    auto mainWindow = Application::GetInstance()->GetMainWindow();
    if (mainWindow)
        mainWindow->OnShow();
}

void ApplicationDispatcherAndroid::OnWindowGotFocus()
{
    auto mainWindow = Application::GetInstance()->GetMainWindow();
    if (mainWindow)
        mainWindow->OnGotFocus();
}

void ApplicationDispatcherAndroid::OnWindowLostFocus()
{
    auto mainWindow = Application::GetInstance()->GetMainWindow();
    if (mainWindow)
        mainWindow->OnLostFocus();
}

void ApplicationDispatcherAndroid::OnWindowHide()
{
    auto mainWindow = Application::GetInstance()->GetMainWindow();
    if (mainWindow)
        mainWindow->OnHide();
}

void ApplicationDispatcherAndroid::OnWindowDestroyed()
{
    auto app = Application::GetInstance();

    std::vector<gptr<Window>> windows;
    app->GetWindows(windows);

    for (auto& win : windows)
    {
        if (auto surface = win->GetSurface())
            surface->ReleaseGraphicsResource();
    }

    auto mainWindow = app->GetMainWindow();
    if (mainWindow)
        mainWindow->OnDestroy();

    processEvents = false;
}

void ApplicationDispatcherAndroid::OnWindowResized()
{
    auto mainWindow = std::dynamic_pointer_cast<WindowAndroid>(Application::GetInstance()->GetMainWindow());
    if (mainWindow && androidApp->window)
    {
        int oldWidth = mainWindow->GetSize().x;
        int oldHeight = mainWindow->GetSize().y;
        int width = ANativeWindow_getWidth(androidApp->window);
        int height = ANativeWindow_getHeight(androidApp->window);

        Console::WriteLine("WINDOW RESIZED (% x %) -> (% x %)", oldWidth, oldHeight, width, height);

        auto newSize = mw::IVec2(width, height);
        mainWindow->SetSizeInternal(newSize);
        mainWindow->OnResize(newSize);
    }
}

void ApplicationDispatcherAndroid::OnConfigurationChanged()
{
    OnWindowResized();
}

void ApplicationDispatcherAndroid::Run(int argc, char* argv[])
{
    assert(androidApp);

    Console::WriteLine("RUN STARTED");

    androidApp->userData = this;
    androidApp->onAppCmd = OnCommand;

    looper = ALooper_forThread();
    run = true;

    while (run)
    {
        auto timeout = GetDispatchTimeout();

        int events;
        struct android_poll_source* source;

        int id = ALooper_pollOnce(timeout, nullptr, &events, (void**)&source);

        if (id >= 0)
        {
            if (source)
                source->process(androidApp, source);

            if (androidApp->destroyRequested != 0) {
                run = false;
            }
        }

        if (processEvents)
        {
            ProcessActions();

            auto now = DispatchClock::now();
            if (continuousDispatchRate > 0 && now >= continuousDispatchWakeTime)
            {
                continuousDispatchWakeTime = now + continuousDispatchInterval;
                InvokeDelegates();
            }
        }
    }

    Console::WriteLine("RUN FINISHED");

    run = false;
}

void ApplicationDispatcherAndroid::ProcessActions()
{
    auto action = GetNextAction();
    while (action)
    {
        InvokeFunction(action);
        action = GetNextAction();
    }
}

gptr<DispatchAction> ApplicationDispatcherAndroid::GetNextAction()
{
    std::unique_lock<std::mutex> lk(mut);

    gptr<DispatchAction> action;

    if (run && !actions.empty() && DispatchClock::now() >= actions.front()->when) {
        action = std::move(actions.front());
        actions.popFront();
    }

    return action;
}

} // system
} // mw
