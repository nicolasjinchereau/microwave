/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#include <functional>
#include <cstdint>
#include <MW/System/ApplicationDispatcher.h>

namespace mw {

class ApplicationDispatcherIOS : public ApplicationDispatcher
{
protected:
    void Wake() override;
public:
    bool wakeMessageQueued = false;
    dispatch_queue_main_t mainQueue = nil;
    void(^processActions)() = nil;
    void(^invokeDelegates)(NSTimer*) = nil;
    NSTimer* continuousDispatchTimer = nil;

    ApplicationDispatcherIOS();
    ~ApplicationDispatcherIOS();

    virtual void Run(int argc, char *argv[]) override;
    virtual void Quit() override;
    virtual void SetContinuousDispatchRate(uint32_t rate) override;
    virtual sptr<DispatchAction> InvokeAsync(
        std::function<void()> function,
        DispatchTime when = DispatchTime()) override;

    void SetReady();
private:
    
    void ProcessActions();
    sptr<DispatchAction> GetNextAction();
    void UpdateActionTimer();
};

}
