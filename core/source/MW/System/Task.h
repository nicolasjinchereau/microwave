/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <utility>
#include <type_traits>
#include <algorithm>
#include <memory>
#include <experimental/coroutine>
#include <condition_variable>
#include <chrono>
#include <MW/System/Awaitable.h>
#include <MW/System/Dispatcher.h>

namespace mw {
inline namespace system {

using milliseconds = std::chrono::milliseconds;

template<class T>
class Task
{
    sptr<IAwaitable<T>> awaitable;
public:
    Task() = default;

    Task(const sptr<IAwaitable<T>>& awaitable)
        : awaitable(awaitable) {}

    Task(const Task&) = default;
    Task(Task&&) noexcept = default;
    ~Task() = default;
    Task& operator=(const Task&) = default;
    Task& operator=(Task&& task) noexcept = default;

    Task& operator=(nullptr_t) {
        awaitable.reset();
        return *this;
    }

    sptr<IAwaiter> GetAwaiter() {
        return std::dynamic_pointer_cast<IAwaiter>(awaitable);
    }

    operator bool() const { return awaitable != nullptr; }
    bool operator!() const { return awaitable == nullptr; }

    bool await_ready() {
        assert(awaitable);
        return awaitable->IsAwaitableReady();
    }

    template<class Promise>
    void await_suspend(std::experimental::coroutine_handle<Promise> caller)
    {
        assert(awaitable);
        awaitable->OnAwaiterSuspended(caller.promise().awaiter.lock());
    }

    T await_resume() {
        assert(awaitable);
        return awaitable->OnAwaiterResumed();
    }

    T GetResult() {
        assert(awaitable);
        return awaitable->WaitForResult();
    }
    
    static Task<void> Delay(milliseconds length);
    static Task<T> GetCompleted();

    struct promise_type;
};

namespace detail {
Task<void> GetDelayTask(milliseconds length);

template<class T>
class CompletedAwaitable : public IAwaitable<T>
{
public:
    virtual bool IsAwaitableReady() override { return true; }
    virtual void OnAwaiterSuspended(const sptr<IAwaiter>& caller) override { assert(0); }
    virtual T OnAwaiterResumed() override { return {}; }
    virtual T WaitForResult() override { return {}; }
};

}

template<class T>
inline Task<void> Task<T>::Delay(milliseconds length) {
    return detail::GetDelayTask(length);
}

template<class T>
inline Task<T> Task<T>::GetCompleted() {
    return Task<T>(spnew<detail::CompletedAwaitable<T>>());
}

template<class T>
class CoroutineAwaitable : public Awaitable<T>, public IAwaiter
{
public:
    std::experimental::coroutine_handle<> state;
    sptr<Dispatcher> owningDispatcher;

    CoroutineAwaitable(
        std::experimental::coroutine_handle<> state,
        const sptr<Dispatcher>& owningDispatcher)
        : state(state),
          owningDispatcher(owningDispatcher)
    {
    }

    ~CoroutineAwaitable()
    {
        owningDispatcher->InvokeAsync([s = state]() mutable {
            s.destroy();
        });
    }

    virtual void Resume() override {
        state.resume();
    }
};

template<class T>
struct promise_base
{
    wptr<CoroutineAwaitable<T>> awaiter;

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
    wptr<CoroutineAwaitable<void>> awaiter;

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
        auto aw = spnew<CoroutineAwaitable<T>>(state, dispatcher);
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
