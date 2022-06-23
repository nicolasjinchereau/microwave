/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.System.Dispatcher;
import Microwave.IO.Terminal;
import Microwave.System.Exception;
import <chrono>;

namespace mw {
inline namespace system {

thread_local gptr<Dispatcher> Dispatcher::_currentDispatcher;

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

gptr<DispatchAction> Dispatcher::InvokeAsync(
    gfunction<void()> function,
    std::chrono::steady_clock::time_point when
)
{
    std::unique_lock<std::mutex> lk(mut);

    auto action = gpnew<DispatchAction>(std::move(function), when);
    sorted_insert(actions, std::move(action), DispatchActionComparison());
    Wake();
    return action;
}

bool Dispatcher::Cancel(const gptr<DispatchAction>& action)
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
                continuousDispatchAction->when = std::chrono::steady_clock::time_point{ std::chrono::steady_clock::duration::zero() };
            }

            continuousDispatchAction = gpnew<DispatchAction>(
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
                continuousDispatchAction->when = std::chrono::steady_clock::time_point{ std::chrono::steady_clock::duration::zero() };
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

void Dispatcher::AddHandler(const gptr<IDispatchHandler>& handler)
{
    std::unique_lock<std::mutex> lk(mut);
    dispatchHandlers.Add(handler);
}

void Dispatcher::RemoveHandler(const gptr<IDispatchHandler>& handler)
{
    std::unique_lock<std::mutex> lk(mut);
    dispatchHandlers.Remove(handler);
}

gptr<Dispatcher> Dispatcher::GetCurrent() {
    return _currentDispatcher;
}

void Dispatcher::SetCurrent(gptr<Dispatcher> dispatcher) {
    _currentDispatcher = dispatcher;
}

void Dispatcher::InvokeFunction(const gptr<DispatchAction>& action)
{
    try
    {
        if (action->function)
            action->function();
    }
    catch (const Exception& ex) {
        writeln("Dispatcher: error invoking dispatch action - ", ex.what());
    }
}

void Dispatcher::InvokeDelegates()
{
    dispatchHandlers.RaiseEvent(mut, &IDispatchHandler::OnDispatch);
}

gptr<DispatchAction> Dispatcher::WaitForNextAction()
{
    std::unique_lock<std::mutex> lk(mut);

    auto wake = std::chrono::steady_clock::time_point::max();
    if (!actions.empty() && actions.front()->when > std::chrono::steady_clock::time_point(std::chrono::microseconds(0)))
        wake = actions.front()->when;

    cv.wait_until(
        lk, wake,
        [this]{ return !run || (!actions.empty() && std::chrono::steady_clock::now() > actions.front()->when); }
    );

    gptr<DispatchAction> action;

    if (run) {
        action = std::move(actions.front());
        actions.erase(actions.begin());
    }

    return action;
}

} // system
} // mw
