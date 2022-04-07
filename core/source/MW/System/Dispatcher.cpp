/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/System/Pointers.h>
#include <MW/System/Dispatcher.h>

namespace mw {
inline namespace system {

thread_local sptr<Dispatcher> Dispatcher::_currentDispatcher;

Dispatcher::Dispatcher()
{
}

Dispatcher::~Dispatcher()
{
}

void Dispatcher::Wake()
{
    cv.notify_one();
}

sptr<DispatchAction> Dispatcher::InvokeAsync(
    std::function<void()> function,
    DispatchTime when
)
{
    std::unique_lock<std::mutex> lk(mut);

    auto action = spnew<DispatchAction>(std::move(function), when);
    sorted_insert(actions, action, DispatchActionComparison());
    Wake();
    return action;
}

bool Dispatcher::Cancel(const sptr<DispatchAction>& action)
{
    std::unique_lock<std::mutex> lk(mut);

    auto it = std::lower_bound(actions.begin(), actions.end(), action, DispatchActionComparison());
    if (it != actions.end()) {
        actions.erase(it);
        return true;
    }

    return false;
}

void Dispatcher::Run()
{
    run = true;
    
    while (run)
    {
        auto action = WaitForNextAction();
        if (action) {
            InvokeFunction(action);
        }
    }
}

void Dispatcher::Quit()
{
    std::unique_lock<std::mutex> lk(mut);
    run = false;
    cv.notify_one();
}

void Dispatcher::SetContinuousDispatchRate(uint32_t rate)
{
    std::unique_lock<std::mutex> lk(mut);

    if(continuousDispatchRate != rate)
    {
        continuousDispatchRate = rate;
        continuousDispatchInterval = std::chrono::milliseconds((2 * 1000 / rate + 1) / 2);
        continuousDispatchWakeTime = std::chrono::steady_clock::now();

        if (continuousDispatchRate > 0)
        {
            if (continuousDispatchAction) {
                continuousDispatchAction->function = nullptr;
                continuousDispatchAction->when = DispatchTime();
            }

            continuousDispatchAction = spnew<DispatchAction>(
                [this]{
                    InvokeDelegates();

                    {
                        std::unique_lock<std::mutex> lk(mut);
                        continuousDispatchWakeTime += continuousDispatchInterval;
                        continuousDispatchAction->when = continuousDispatchWakeTime;
                        sorted_insert(actions, continuousDispatchAction, DispatchActionComparison());
                        Wake();
                    }
                },
                continuousDispatchWakeTime
            );
        }
        else
        {
            if (continuousDispatchAction) {
                continuousDispatchAction->function = nullptr;
                continuousDispatchAction->when = DispatchTime();
                continuousDispatchAction = nullptr;
            }
        }

        Wake();
    }
}

uint32_t Dispatcher::GetContinuousDispatchRate() const
{
    std::unique_lock<std::mutex> lk(mut);
    return continuousDispatchRate;
}

void Dispatcher::AddHandler(const sptr<IDispatchHandler>& handler)
{
    std::unique_lock<std::mutex> lk(mut);
    dispatchHandlers.Add(handler);
}

void Dispatcher::RemoveHandler(const sptr<IDispatchHandler>& handler)
{
    std::unique_lock<std::mutex> lk(mut);
    dispatchHandlers.Remove(handler);
}

sptr<Dispatcher> Dispatcher::GetCurrent() {
    return _currentDispatcher;
}

void Dispatcher::SetCurrent(sptr<Dispatcher> dispatcher) {
    _currentDispatcher = dispatcher;
}

void Dispatcher::InvokeFunction(const sptr<DispatchAction>& action)
{
    try
    {
        if (action->function)
            action->function();
    }
    catch (std::exception & ex) {
        Console::WriteLine("Dispatcher: error invoking dispatch action - %", ex.what());
    }
}

void Dispatcher::InvokeDelegates()
{
    dispatchHandlers.RaiseEvent(mut, &IDispatchHandler::OnDispatch);
}

sptr<DispatchAction> Dispatcher::WaitForNextAction()
{
    std::unique_lock<std::mutex> lk(mut);

    auto wake = DispatchTime::max();
    if (!actions.empty() && actions.front()->when > DispatchTime(std::chrono::microseconds(0)))
        wake = actions.front()->when;

    cv.wait_until(
        lk, wake,
        [this]{ return !run || (!actions.empty() && DispatchClock::now() > actions.front()->when); }
    );
    
    sptr<DispatchAction> action;

    if (run) {
        action = std::move(actions.front());
        actions.pop_front();
    }

    return action;
}

} // system
} // mw
