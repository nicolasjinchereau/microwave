/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.System.Internal.ApplicationDispatcherWindows;
import <MW/System/Internal/PlatformHeaders.h>;

constexpr int WM_DISPATCHER_QUIT = WM_APP + 1;
constexpr int WM_DISPATCHER_WAKE = WM_APP + 100;

namespace mw {
inline namespace system {

sptr<ApplicationDispatcher> ApplicationDispatcher::New() {
    return spnew<ApplicationDispatcherWindows>();
}

void ApplicationDispatcherWindows::Wake()
{
    if (!wakeMessageQueued)
    {
        wakeMessageQueued = true;
        PostMessage(hWndMsg, WM_DISPATCHER_WAKE, 0, 0);
    }
}

ApplicationDispatcherWindows::ApplicationDispatcherWindows()
{
    CreateMessageWindow();
}

ApplicationDispatcherWindows::~ApplicationDispatcherWindows()
{
    if (hWndMsg)
        DestroyWindow(hWndMsg);
}

sptr<DispatchAction> ApplicationDispatcherWindows::InvokeAsync(
    std::function<void()> function, DispatchTime when
)
{
    std::unique_lock<std::mutex> lk(mut);

    auto action = spnew<DispatchAction>(std::move(function), when);
    sorted_insert(actions, action, DispatchActionComparison());
    Wake();
    return action;
}

//BOOL GetMessageWithTimeout(MSG *msg, UINT timeout)
//{
//    UINT_PTR timerId = SetTimer(NULL, NULL, timeout, NULL);
//    BOOL res = GetMessage(msg);
//    KillTimer(NULL, timerId);
//
//    if (!res)
//        return FALSE;
//
//    if (msg->message == WM_TIMER && msg->hwnd == NULL && msg->wParam == timerId)
//        return FALSE; //TIMEOUT! You could call SetLastError() or something...
//
//    return TRUE;
//}

void ApplicationDispatcherWindows::Run(int argc, char* argv[])
{
    run = true;

    MSG msg;

    while (run)
    {
        if(continuousDispatchRate > 0)
        {
            if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                if(msg.message == WM_QUIT)
                    break;

                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
            {
                auto timeout = std::chrono::duration_cast<std::chrono::milliseconds>(
                    continuousDispatchWakeTime - std::chrono::steady_clock::now()).count();

                if (timeout > 0)
                {
                    auto ret = MsgWaitForMultipleObjects(0, NULL, FALSE, (DWORD)timeout, QS_ALLINPUT);
                    auto inputAvailable = (ret != WAIT_TIMEOUT);

                    if (inputAvailable)
                        continue;
                }

                continuousDispatchWakeTime = std::chrono::steady_clock::now() + continuousDispatchInterval;
                InvokeDelegates();
            }
        }
        else
        {
            if(!GetMessage(&msg, NULL, 0, 0))
                break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

void ApplicationDispatcherWindows::Quit() {
    PostMessage(hWndMsg, WM_DISPATCHER_QUIT, 0, 0);
}

void ApplicationDispatcherWindows::CreateMessageWindow()
{
    const char* name = "DispatcherMessageTarget";
    HINSTANCE hInstance = GetModuleHandle(NULL);

    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = 0;
    wcex.lpfnWndProc = &ApplicationDispatcherWindows::CreateWndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = sizeof(ApplicationDispatcherWindows*);
    wcex.hInstance = hInstance;
    wcex.hIcon = NULL;
    wcex.hCursor = NULL;
    wcex.hbrBackground = NULL;
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = name;
    wcex.hIconSm = NULL;

    if (!RegisterClassEx(&wcex))
        throw std::runtime_error("failed to register window class for dispatcher message target window");

    hWndMsg = CreateWindow(name, name, WS_DISABLED, 0, 0, 32, 32, HWND_MESSAGE, NULL, hInstance, this);
    if (!hWndMsg)
        throw std::runtime_error("failed to create target window for dispatcher");
}

LRESULT CALLBACK ApplicationDispatcherWindows::CreateWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_CREATE)
    {
        CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
        ApplicationDispatcherWindows* disp = (ApplicationDispatcherWindows*)cs->lpCreateParams;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)disp);
        SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)&ApplicationDispatcherWindows::StaticWndProc);
        return disp->InstanceWndProc(hWnd, msg, wParam, lParam);
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK ApplicationDispatcherWindows::StaticWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    auto dispatcher = (ApplicationDispatcherWindows*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    return dispatcher->InstanceWndProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK ApplicationDispatcherWindows::InstanceWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_TIMER:
        if (wParam != timerID)
            break;

        timerID = 0;

    case WM_DISPATCHER_WAKE:
        wakeMessageQueued = false;
        ProcessActions();
        return 0;

    case WM_DISPATCHER_QUIT:
        run = false;
        isQuitting = true;
        return 0;

    default:
        break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

void ApplicationDispatcherWindows::ProcessActions()
{
    auto action = GetNextAction();
    while (action)
    {
        InvokeFunction(action);
        action = GetNextAction();
    }
}

sptr<DispatchAction> ApplicationDispatcherWindows::GetNextAction()
{
    std::unique_lock<std::mutex> lk(mut);

    sptr<DispatchAction> action;

    if (run && !actions.empty() && DispatchClock::now() >= actions.front()->when) {
        action = std::move(actions.front());
        actions.pop_front();
    }

    UpdateActionTimer();

    return action;
}

void ApplicationDispatcherWindows::UpdateActionTimer()
{
    if (timerID != 0) {
        KillTimer(hWndMsg, (UINT_PTR)timerID);
        timerID = 0;
    }

    if (!actions.empty())
    {
        auto now = DispatchClock::now();
        auto when = actions.front()->when;

        // queue timer for next action if needed
        if (when > now)
        {
            // TODO: figure out what to do about USER_TIMER_MINIMUM (SetTimer rounds up to this - 10ms)
            auto timeout = std::chrono::duration_cast<std::chrono::milliseconds>(when - now).count();
            timerID = (uint64_t)SetTimer(hWndMsg, (UINT_PTR)1001, (UINT)timeout, NULL);
        }
    }
}

} // system
} // mw
