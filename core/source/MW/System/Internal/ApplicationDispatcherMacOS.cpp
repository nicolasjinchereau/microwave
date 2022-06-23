/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.System.Internal.ApplicationDispatcherMacOS;
import <stdexcept>;

@interface AppDelegate : NSObject<NSApplicationDelegate>
{
}
@end

@implementation AppDelegate
-(void)applicationDidFinishLaunching:(NSNotification *)notification
{
}
@end

namespace mw {
inline namespace system {

gptr<ApplicationDispatcher> ApplicationDispatcher::New() {
    return gpnew<ApplicationDispatcherMacOS>();
}

void ApplicationDispatcherMacOS::Wake()
{
    if(!wakeMessageQueued)
    {
        wakeMessageQueued = true;
        dispatch_async(mainQueue, processActions);
    }
}

ApplicationDispatcherMacOS::ApplicationDispatcherMacOS()
{
    mainQueue = dispatch_get_main_queue();
    
    processActions = ^{
        wakeMessageQueued = false;
        ProcessActions();
    };
    
    invokeDelegates = ^(NSTimer*){
        InvokeDelegates();
    };
}

ApplicationDispatcherMacOS::~ApplicationDispatcherMacOS()
{
}

gptr<DispatchAction> ApplicationDispatcherMacOS::InvokeAsync(
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

void ApplicationDispatcherMacOS::Run(int argc, char *argv[])
{
    run = true;
    
    @autoreleasepool {
        appDelegate = [[AppDelegate alloc] init];
        [[NSApplication sharedApplication] setDelegate:appDelegate];
        [NSApp run];
    }
}

void ApplicationDispatcherMacOS::Quit() {
    [NSApp terminate:appDelegate];
}

void ApplicationDispatcherMacOS::SetContinuousDispatchRate(uint32_t rate)
{
    std::unique_lock<std::mutex> lk(mut);

    if(continuousDispatchRate != rate)
    {
        continuousDispatchRate = rate;
        continuousDispatchInterval = std::chrono::milliseconds((2 * 1000 / rate + 1) / 2);
        continuousDispatchWakeTime = std::chrono::steady_clock::now();
        
        if(continuousDispatchRate > 0)
        {
            if(continuousDispatchTimer)
                [continuousDispatchTimer invalidate];
            
            float interval = std::chrono::duration_cast<std::chrono::duration<float>>(continuousDispatchInterval).count();
            continuousDispatchTimer = [NSTimer scheduledTimerWithTimeInterval:interval repeats:YES block:invokeDelegates];
        }
        else
        {
            if(continuousDispatchTimer)
            {
                [continuousDispatchTimer invalidate];
                continuousDispatchTimer = nil;
            }
        }
    }
}

void ApplicationDispatcherMacOS::ProcessActions()
{
    auto action = GetNextAction();
    while (action)
    {
        InvokeFunction(action);
        action = GetNextAction();
    }
}

gptr<DispatchAction> ApplicationDispatcherMacOS::GetNextAction()
{
    std::unique_lock<std::mutex> lk(mut);

    gptr<DispatchAction> action;

    if (run && !actions.empty() && DispatchClock::now() >= actions.front()->when) {
        action = std::move(actions.front());
        actions.popFront();
    }

    UpdateActionTimer();

    return action;
}

void ApplicationDispatcherMacOS::UpdateActionTimer()
{
    // TODO: stop existing timer somehow
    
    if (run && !actions.empty())
    {
        auto now = DispatchClock::now();
        auto when = actions.front()->when;

        // queue timer for next action if needed
        if (when > now)
        {
            auto timeout = std::chrono::duration_cast<std::chrono::milliseconds>(when - now).count();
            
            dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(timeout * NSEC_PER_MSEC)), dispatch_get_main_queue(), ^{
                ProcessActions();
            });
        }
    }
}

} // system
} // mw
