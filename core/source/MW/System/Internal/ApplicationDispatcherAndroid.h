/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <functional>
#include <cstdint>
#include <android_native_app_glue.h>
#include <android/native_window.h>
#include <android/native_activity.h>
#include <android/looper.h>
#include <MW/System/ApplicationDispatcher.h>

namespace mw {

class ApplicationDispatcherAndroid : public ApplicationDispatcher
{
protected:
    void Wake() override;

public:
    ApplicationDispatcherAndroid();
    ~ApplicationDispatcherAndroid();

    virtual void Run(int argc, char *argv[]) override;
    virtual void Quit() override;
    virtual sptr<DispatchAction> InvokeAsync(
        std::function<void()> function,
        DispatchTime when = DispatchTime()) override;

private:
    long long GetDispatchTimeout();
    void ProcessActions();
    sptr<DispatchAction> GetNextAction();

    static void OnCommand(android_app *pApp, int32_t cmd);
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

}
