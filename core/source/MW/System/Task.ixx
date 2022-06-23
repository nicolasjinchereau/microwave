/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.System.Task;
import Microwave.System.Awaitable;
import Microwave.System.IAwaitable;
import Microwave.System.IAwaiter;
import Microwave.System.Dispatcher;
import Microwave.System.Exception;
import <MW/System/Debug.h>;
import <algorithm>;
import <chrono>;
import <condition_variable>;
import <cstddef>;
import <experimental/coroutine>;
import <memory>;
import <type_traits>;
import <utility>;

export namespace mw {
inline namespace system {

template<class T>
class Task
{
    gptr<IAwaitable<T>> awaitable;
public:
    Task() = default;

    Task(const gptr<IAwaitable<T>>& awaitable)
        : awaitable(awaitable) {}

    Task(const Task&) = default;
    Task(Task&&) noexcept = default;
    ~Task() = default;
    Task& operator=(const Task&) = default;
    Task& operator=(Task&& task) noexcept = default;

    Task& operator=(std::nullptr_t) {
        awaitable.reset();
        return *this;
    }

    gptr<IAwaiter> GetAwaiter() {
        return gpcast<IAwaiter>(awaitable);
    }

    operator bool() const { return awaitable != nullptr; }
    bool operator!() const { return awaitable == nullptr; }

    bool await_ready() {
        Assert(awaitable);
        return awaitable->IsAwaitableReady();
    }

    template<class Promise>
    void await_suspend(std::experimental::coroutine_handle<Promise> caller)
    {
        Assert(awaitable);
        awaitable->OnAwaiterSuspended(caller.promise().awaiter.lock());
    }

    T await_resume() {
        Assert(awaitable);
        return awaitable->OnAwaiterResumed();
    }

    T GetResult() {
        Assert(awaitable);
        return awaitable->WaitForResult();
    }
    
    static Task<void> Delay(std::chrono::milliseconds length);
    static Task<T> GetCompleted();

    struct promise_type;
};

namespace detail {
Task<void> GetDelayTask(std::chrono::milliseconds length);

template<class T>
class CompletedAwaitable : public IAwaitable<T>
{
public:
    virtual bool IsAwaitableReady() override { return true; }
    virtual void OnAwaiterSuspended(const gptr<IAwaiter>& caller) override { Assert(0); }
    virtual T OnAwaiterResumed() override { return {}; }
    virtual T WaitForResult() override { return {}; }
};

}

template<class T>
inline Task<void> Task<T>::Delay(std::chrono::milliseconds length) {
    return detail::GetDelayTask(length);
}

template<class T>
inline Task<T> Task<T>::GetCompleted() {
    return Task<T>(gpnew<detail::CompletedAwaitable<T>>());
}

template<class T>
class CoroutineAwaitable : public Awaitable<T>, public IAwaiter
{
public:
    std::experimental::coroutine_handle<> state;
    wgptr<Dispatcher> owningDispatcher;

    CoroutineAwaitable(
        std::experimental::coroutine_handle<> state,
        const gptr<Dispatcher>& owningDispatcher)
        : state(state),
          owningDispatcher(owningDispatcher)
    {
    }

    ~CoroutineAwaitable()
    {
        if(auto disp = owningDispatcher.lock())
        {
            disp->InvokeAsync([s = state]() mutable {
                s.destroy();
            });
        }
    }

    virtual void Resume() override {
        state.resume();
    }
};

template<class T>
struct promise_base
{
    wgptr<CoroutineAwaitable<T>> awaiter;

    template<class U>
    void return_value(U&& value)
    {
        if (auto aw = this->awaiter.lock())
            aw->SetCompleted(std::forward<U>(value));
    }
};

template<>
struct promise_base<void>
{
    wgptr<CoroutineAwaitable<void>> awaiter;

    void return_void()
    {
        if (auto aw = this->awaiter.lock())
            aw->SetCompleted();
    }
};

template<class T>
struct Task<T>::promise_type : promise_base<T>
{
    Task<T> get_return_object()
    {
        using std::experimental::coroutine_handle;

        auto handle = coroutine_handle<promise_type>::from_promise(*this);
        auto state = coroutine_handle<>::from_address(handle.address());
        auto dispatcher = Dispatcher::GetCurrent();
        auto aw = gpnew<CoroutineAwaitable<T>>(state, dispatcher);
        this->awaiter = aw;
        return Task<T>(aw);
    }

    auto initial_suspend() const {
        return std::experimental::suspend_never{};
    }

    auto final_suspend() const {
        return std::experimental::suspend_always{};
    }

    void unhandled_exception()
    {
        if (auto aw = this->awaiter.lock())
            aw->SetException(std::current_exception());
    }
};

} // system
} // mw
