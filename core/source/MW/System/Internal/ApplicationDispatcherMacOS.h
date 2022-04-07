/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#include <functional>
#include <cstdint>
#include <MW/System/ApplicationDispatcher.h>

@class AppDelegate;

namespace mw {

class ApplicationDispatcherMacOS : public ApplicationDispatcher
{
protected:
    void Wake() override;
public:
    AppDelegate* appDelegate = nil;
    bool wakeMessageQueued = false;
    dispatch_queue_main_t mainQueue = nil;
    void(^processActions)() = nil;
    void(^invokeDelegates)(NSTimer*) = nil;
    NSTimer* continuousDispatchTimer = nil;

    ApplicationDispatcherMacOS();
    ~ApplicationDispatcherMacOS();

    virtual void Run(int argc, char *argv[]) override;
    virtual void Quit() override;
    virtual void SetContinuousDispatchRate(uint32_t rate) override;
    virtual sptr<DispatchAction> InvokeAsync(
        std::function<void()> function,
        DispatchTime when = DispatchTime()) override;
private:
    
    void ProcessActions();
    sptr<DispatchAction> GetNextAction();
    void UpdateActionTimer();
};

}
