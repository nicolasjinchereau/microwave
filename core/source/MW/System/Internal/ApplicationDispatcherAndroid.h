/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.System.Internal.ApplicationDispatcherAndroid;
import Microwave.System.ApplicationDispatcher;
import <android_native_app_glue.h>;
import <android/native_window.h>;
import <android/native_activity.h>;
import <android/looper.h>;
import <functional>;
import <cstdint>;

export namespace mw {
inline namespace system {

class ApplicationDispatcherAndroid : public ApplicationDispatcher
{
protected:
    void Wake() override;

public:
    ApplicationDispatcherAndroid();
    ~ApplicationDispatcherAndroid();

    virtual void Run(int argc, char* argv[]) override;
    virtual void Quit() override;
    virtual gptr<DispatchAction> InvokeAsync(
        std::function<void()> function,
        DispatchTime when = DispatchTime()) override;

private:
    long long GetDispatchTimeout();
    void ProcessActions();
    gptr<DispatchAction> GetNextAction();

    static void OnCommand(android_app* pApp, int32_t cmd);
    static int32_t OnInput(struct android_app* app, AInputEvent* event);

    void OnWindowCreated();
    void OnWindowShow();
    void OnWindowGotFocus();
    void OnWindowLostFocus();
    void OnWindowHide();
    void OnWindowDestroyed();

    void OnWindowResized();
    void OnConfigurationChanged();

    ALooper* looper = nullptr;
    bool processEvents = false;
};

} // system
} // mw
