/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.System.Internal.ApplicationDispatcherWindows;
import Microwave.System.ApplicationDispatcher;
import Microwave.System.Pointers;
import <chrono>;
import <cstdint>;
import <functional>;
import <stdexcept>;
import <mutex>;
import <MW/System/Internal/PlatformHeaders.h>;

export namespace mw {
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
        std::chrono::steady_clock::time_point when = std::chrono::steady_clock::time_point{ std::chrono::steady_clock::duration::zero() }
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
