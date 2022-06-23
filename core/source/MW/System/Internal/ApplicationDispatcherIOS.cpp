/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.System.Internal.ApplicationDispatcherIOS;
import <stdexcept>;

namespace detail {
    thread_local mw::gptr<mw::ApplicationDispatcherIOS> _mainThreadDispatcher;
}

@interface AppDelegate : UIResponder<UIApplicationDelegate>
@end

@implementation AppDelegate
- (BOOL)application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary*)launchOptions {
    detail::_mainThreadDispatcher->SetReady();
    return YES;
}
- (void)applicationWillTerminate:(UIApplication *)application
{
}
@end

namespace mw {
inline namespace system {

gptr<ApplicationDispatcher> ApplicationDispatcher::New() {
    detail::_mainThreadDispatcher = gpnew<ApplicationDispatcherIOS>();
    return detail::_mainThreadDispatcher;
}

void ApplicationDispatcherIOS::Wake()
{
    if(run && !wakeMessageQueued)
    {
        wakeMessageQueued = true;
        dispatch_async(mainQueue, processActions);
    }
}

ApplicationDispatcherIOS::ApplicationDispatcherIOS()
{
    mainQueue = dispatch_get_main_queue();
    
    processActions = ^{
        wakeMessageQueued = false;
        ProcessActions();
    };
    
    invokeDelegates = ^(NSTimer* timer){
        InvokeDelegates();
    };
}

ApplicationDispatcherIOS::~ApplicationDispatcherIOS()
{
}

gptr<DispatchAction> ApplicationDispatcherIOS::InvokeAsync(
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

void ApplicationDispatcherIOS::SetReady()
{
    run = true;
    Wake();
}

void ApplicationDispatcherIOS::Run(int argc, char *argv[])
{
    @autoreleasepool
    {
        UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
    }
}

void ApplicationDispatcherIOS::Quit() {
    exit(0);
}

void ApplicationDispatcherIOS::SetContinuousDispatchRate(uint32_t rate)
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

void ApplicationDispatcherIOS::ProcessActions()
{
    auto action = GetNextAction();
    while (action)
    {
        InvokeFunction(action);
        action = GetNextAction();
    }
}

gptr<DispatchAction> ApplicationDispatcherIOS::GetNextAction()
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

void ApplicationDispatcherIOS::UpdateActionTimer()
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
