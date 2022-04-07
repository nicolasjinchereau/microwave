/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <cstdint>
#include <vector>
#include <chrono>
#include <mutex>
#include <atomic>
#include <functional>
#include <MW/System/Pointers.h>
#include <MW/System/Console.h>
#include <MW/System/EventHandlerList.h>
#include <deque>

namespace mw {
inline namespace system {

using DispatchClock = std::chrono::steady_clock;
using DispatchTime = std::chrono::steady_clock::time_point;

struct DispatchAction
{
    typedef std::chrono::microseconds Duration;

    std::function<void()> function;
    DispatchTime when = DispatchTime();

    DispatchAction() {}

    template<class Func>
    DispatchAction(
        Func&& function,
        DispatchTime when = DispatchTime()
    ) : function(std::forward<Func>(function)), when(when) {}
};

class IDispatchHandler
{
public:
    virtual void OnDispatch() = 0;
};

class Dispatcher : public std::enable_shared_from_this<Dispatcher>
{
protected:
    struct DispatchActionComparison {
        bool operator()(const sptr<DispatchAction>& left, const sptr<DispatchAction>& right) const {
            return left->when < right->when;
        }
    };

    template<class T, class S, class Compare>
    inline static auto sorted_insert(std::deque<T>& cont, S&& val, Compare comp)
    {
        auto it = std::upper_bound(cont.begin(), cont.end(), val, comp);
        return cont.insert(it, std::forward<S>(val));
    }

    virtual void Wake();

public:
    Dispatcher();
    ~Dispatcher();

    virtual sptr<DispatchAction> InvokeAsync(
        std::function<void()> function,
        DispatchTime when = DispatchTime()
    );

    virtual bool Cancel(const sptr<DispatchAction>& action);
    virtual void Run();
    virtual void Quit();

    virtual void SetContinuousDispatchRate(uint32_t rate);
    virtual uint32_t GetContinuousDispatchRate() const;

    virtual void AddHandler(const sptr<IDispatchHandler>& handler);
    virtual void RemoveHandler(const sptr<IDispatchHandler>& handler);
    
    static void SetCurrent(sptr<Dispatcher> dispatcher);
    static sptr<Dispatcher> GetCurrent();

protected:

    mutable std::mutex mut;
    mutable std::condition_variable cv;
    std::atomic<bool> run = false;
    std::deque<sptr<DispatchAction>> actions;
    std::chrono::steady_clock::time_point continuousDispatchWakeTime;
    std::chrono::milliseconds continuousDispatchInterval = std::chrono::milliseconds(0);
    uint32_t continuousDispatchRate = 0;
    
    EventHandlerList<IDispatchHandler> dispatchHandlers;
    sptr<DispatchAction> continuousDispatchAction;

    void InvokeFunction(const sptr<DispatchAction>& action);
    void InvokeDelegates();
    sptr<DispatchAction> WaitForNextAction();

    static thread_local sptr<Dispatcher> _currentDispatcher;
};

} // system
} // mw
