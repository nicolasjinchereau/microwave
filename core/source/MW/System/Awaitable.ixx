/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.System.Awaitable;
import Microwave.System.IAwaitable;
import Microwave.System.IAwaiter;
import Microwave.System.Dispatcher;
import Microwave.System.Exception;
import Microwave.System.Pointers;
import <MW/System/Debug.h>;
import <condition_variable>;
import <exception>;
import <experimental/coroutine>;
import <mutex>;
import <stdexcept>;
import <utility>;

export namespace mw {
inline namespace system {

template<class T>
struct AwaitableResult;

template<>
struct AwaitableResult<void>
{
    std::exception_ptr ex;
};

template<class T>
struct AwaitableResult : AwaitableResult<void>
{
    T value;
};

struct AwaitingAwaiter
{
    gptr<IAwaiter> awaiter;
    gptr<Dispatcher> dispatcher;
};

template<class T>
class Awaitable : public IAwaitable<T>
{
public:
    std::mutex mut;
    std::condition_variable cond;
    bool finished = false;
    AwaitableResult<T> result;
    gvector<AwaitingAwaiter> awaiters;

    virtual bool IsAwaitableReady() override
    {
        std::lock_guard<std::mutex> lk(mut);
        if (finished && result.ex) std::rethrow_exception(result.ex);
        return finished;
    }

    virtual void OnAwaiterSuspended(const gptr<IAwaiter>& caller) override
    {
        std::lock_guard<std::mutex> lk(mut);

        Assert(caller);
        
        if (finished)
        {
            if (result.ex) std::rethrow_exception(result.ex);

            auto disp = Dispatcher::GetCurrent();
            disp->InvokeAsync([sc = caller]() mutable {
                sc->Resume();
            });
        }
        else
        {
            awaiters.push_back({ caller, Dispatcher::GetCurrent() });
        }
    }

    virtual T OnAwaiterResumed() override
    {
        std::lock_guard<std::mutex> lk(mut);
        if (result.ex) std::rethrow_exception(result.ex);

        if constexpr (!std::is_void_v<T>) {
            return result.value;
        }
    }

    virtual T WaitForResult() override
    {
        std::unique_lock<std::mutex> lk(mut);
        cond.wait(lk, [this] { return finished; });
        if (result.ex) std::rethrow_exception(result.ex);
        
        if constexpr (!std::is_void_v<T>) {
            return result.value;
        }
    }

    template<typename... Args> requires (std::is_void_v<T> == (sizeof...(Args) == 0))
    void SetCompleted(Args&&... args)
    {
        std::lock_guard<std::mutex> lk(mut);

        if constexpr (!std::is_void_v<T>) {
            result.value = T{ std::forward<Args>(args)... };
        }

        finished = true;
        cond.notify_all();

        for(auto& awaiter : awaiters)
        {
            awaiter.dispatcher->InvokeAsync([sc = awaiter.awaiter]() mutable {
                sc->Resume();
            });
        }

        awaiters.clear();
    }

    void SetException(const std::exception_ptr& ex)
    {
        std::lock_guard<std::mutex> lk(mut);
        result.ex = ex;
        finished = true;
        cond.notify_all();

        for (auto& awaiter : awaiters)
        {
            awaiter.dispatcher->InvokeAsync([sc = awaiter.awaiter]() mutable {
                sc->Resume();
            });
        }

        awaiters.clear();
    }
};

} // system
} // mw
