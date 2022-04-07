/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <Windows.h>
#include <chrono>
#include <stdexcept>
#include <mutex>
#include <MW/System/ApplicationDispatcher.h>

namespace mw {
inline namespace system {

class ApplicationDispatcherWindows : public ApplicationDispatcher
{
    HWND hWndMsg = NULL;
    bool isQuitting = false;
    bool wakeMessageQueued = false;
    uint64_t timerID = 0;

protected:
    void Wake() override;

public:
    ApplicationDispatcherWindows();
    ~ApplicationDispatcherWindows();

    virtual sptr<DispatchAction> InvokeAsync(
        std::function<void()> function,
        DispatchTime when = DispatchTime()
    ) override;

    virtual void Run(int argc, char* argv[]) override;
    virtual void Quit() override;

private:
    void CreateMessageWindow();
    static LRESULT CALLBACK CreateWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT CALLBACK InstanceWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void ProcessActions();
    sptr<DispatchAction> GetNextAction();
    void UpdateActionTimer();
};

} // system
} // mw
