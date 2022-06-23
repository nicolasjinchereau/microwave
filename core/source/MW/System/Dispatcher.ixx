/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.System.Dispatcher;
import Microwave.System.Pointers;
import Microwave.System.EventHandlerList;
import <algorithm>;
import <atomic>;
import <chrono>;
import <condition_variable>;
import <cstdint>;
import <deque>;
import <functional>;
import <mutex>;
import <utility>;
import <vector>;

export namespace mw {
inline namespace system {

struct DispatchAction
{
    gfunction<void()> function;
    std::chrono::steady_clock::time_point when{};

    DispatchAction(){}

    template<class Func>
    DispatchAction(
        Func&& function,
        std::chrono::steady_clock::time_point when = {}
    ) : function(std::forward<Func>(function)), when(when) {}
};

class IDispatchHandler
{
public:
    virtual void OnDispatch() = 0;
};

class Dispatcher : public sp_from_this<Dispatcher>
{
protected:
    struct DispatchActionComparison {
        bool operator()(const gptr<DispatchAction>& left, const gptr<DispatchAction>& right) const {
            return left->when < right->when;
        }
    };

    template<class T, class S, class Compare>
    inline static auto sorted_insert(gvector<T>& cont, S&& val, Compare comp)
    {
        auto it = std::upper_bound(cont.begin(), cont.end(), val, comp);
        return cont.insert(it, std::forward<S>(val));
    }

    virtual void Wake();

public:
    Dispatcher();
    ~Dispatcher();

    virtual gptr<DispatchAction> InvokeAsync(
        gfunction<void()> function,
        std::chrono::steady_clock::time_point when = std::chrono::steady_clock::time_point{ std::chrono::steady_clock::duration::zero() }
    );

    virtual bool Cancel(const gptr<DispatchAction>& action);
    virtual void Run();
    virtual void Quit();

    virtual void SetContinuousDispatchRate(uint32_t rate);
    virtual uint32_t GetContinuousDispatchRate() const;

    virtual void AddHandler(const gptr<IDispatchHandler>& handler);
    virtual void RemoveHandler(const gptr<IDispatchHandler>& handler);
    
    static void SetCurrent(gptr<Dispatcher> dispatcher);
    static gptr<Dispatcher> GetCurrent();

protected:

    mutable std::mutex mut;
    mutable std::condition_variable cv;
    std::atomic<bool> run = false;
    gvector<gptr<DispatchAction>> actions;
    std::chrono::steady_clock::time_point continuousDispatchWakeTime;
    std::chrono::milliseconds continuousDispatchInterval = std::chrono::milliseconds(0);
    uint32_t continuousDispatchRate = 0;
    
    EventHandlerList<IDispatchHandler> dispatchHandlers;
    gptr<DispatchAction> continuousDispatchAction;

    void InvokeFunction(const gptr<DispatchAction>& action);
    void InvokeDelegates();
    gptr<DispatchAction> WaitForNextAction();

    static thread_local gptr<Dispatcher> _currentDispatcher;
};

} // system
} // mw
