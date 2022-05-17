/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.SceneGraph.Coroutine;
import Microwave.SceneGraph.Scene;
import Microwave.SceneGraph.Node;
import Microwave.System.Clock;
import Microwave.System.Pointers;
import <cassert>;
import <cstdint>;
import <exception>;
import <experimental/coroutine>;
import <functional>;
import <type_traits>;
import <variant>;
import <vector>;

using std::experimental::coroutine_handle;
using std::experimental::suspend_always;
using std::experimental::suspend_never;

export namespace mw {
inline namespace scene {

enum class WaitType
{
    Next,
    Seconds,
    Predicate,
    Cancelled
};

class Wait
{
    struct Next {};
    struct Cancelled {};
    std::variant<Next, float, std::function<bool()>, Cancelled> value;

    Wait(float seconds) : value(seconds) {}

    template<class Fun> requires std::is_invocable_r_v<bool, Fun>
    Wait(Fun&& until) : value(std::forward<Fun>(until)) {}

public:
    Wait() {}
    Wait(const Wait&) = delete;
    Wait& operator=(const Wait&) = delete;
    Wait(Wait&&) = default;
    Wait& operator=(Wait&&) = default;

    static Wait Next() {
        return { };
    }

    static Wait Seconds(float seconds) {
        return { seconds };
    }

    template<class Fun> requires std::is_invocable_r_v<bool, Fun>
    static Wait Until(Fun&& until) {
        return { std::forward<Fun>(until) };
    }

    WaitType GetType() const {
        return (WaitType)value.index();
    }

    float GetSeconds() const {
        assert(GetType() == WaitType::Seconds);
        return std::get<float>(value);
    }

    void SetSeconds(float seconds) {
        assert(GetType() == WaitType::Seconds);
        value = seconds;
    }

    bool EvaluatePredicate() const {
        assert(GetType() == WaitType::Predicate);
        return std::get<std::function<bool()>>(value)();
    }

    void Cancel() {
        value = Cancelled();
    }
};

struct Coroutine;

namespace detail
{
struct CoroutinePromise
{
    size_t refCount = 0;
    Wait wait;
    std::exception_ptr ex{};

    Coroutine get_return_object();

    auto initial_suspend() {
        return suspend_always{};
    }

    auto yield_value(Wait&& wait) {
        this->wait = std::move(wait);
        return suspend_always{};
    }

    auto final_suspend() noexcept {
        this->wait.Cancel();
        return suspend_always{};
    }

    void return_void() {
        this->wait.Cancel();
    }

    void unhandled_exception() {
        ex = std::current_exception();
    }
};
} // detail

struct Coroutine
{
    typedef detail::CoroutinePromise promise_type;
    friend promise_type;

    coroutine_handle<promise_type> handle{};

    Coroutine(coroutine_handle<promise_type> handle = nullptr)
        : handle(handle)
    {
        IncRef();
    }

    void IncRef()
    {
        if (handle)
            ++handle.promise().refCount;
    }

    void DecRef()
    {
        if (handle && --handle.promise().refCount == 0)
            handle.destroy();
    }
public:

    ~Coroutine() {
        DecRef();
    }

    Coroutine(const Coroutine& other)
    {
        handle = other.handle;
        IncRef();
    }

    Coroutine(Coroutine&& other) noexcept
    {
        DecRef();
        handle = other.handle;
        other.handle = nullptr;
    }

    Coroutine& operator=(const Coroutine& other)
    {
        DecRef();
        handle = other.handle;
        IncRef();
        return *this;
    }

    Coroutine& operator=(Coroutine&& other) noexcept
    {
        DecRef();
        handle = other.handle;
        other.handle = nullptr;
        return *this;
    }

    Coroutine& operator=(std::nullptr_t) noexcept
    {
        DecRef();
        handle = nullptr;
        return *this;
    }

    void Cancel() {
        assert(handle);
        handle.promise().wait.Cancel();
    }

    bool IsCancelled() const {
        assert(handle);
        return handle.promise().wait.GetType() == WaitType::Cancelled;
    }

    bool Step(const sptr<Clock>& clock)
    {
        assert(handle);
        
        if (IsCancelled())
            return false;

        bool ready = false;

        auto& wait = handle.promise().wait;
        auto waitType = wait.GetType();

        if (waitType == WaitType::Next)
        {
            ready = true;
        }
        else if (waitType == WaitType::Seconds)
        {
            auto now = clock->GetTime();
            auto resumeTime = wait.GetSeconds();
            ready = (now >= resumeTime); 
        }
        else if (waitType == WaitType::Predicate)
        {
            ready = wait.EvaluatePredicate();
        }

        if(!ready)
            return true;

        handle.resume();

        if (auto& ex = handle.promise().ex)
            std::rethrow_exception(ex);

        auto updatedWaitType = handle.promise().wait.GetType();
        if (updatedWaitType == WaitType::Seconds)
        {
            float length = handle.promise().wait.GetSeconds();
            float wake = clock->GetTime() + length;
            handle.promise().wait.SetSeconds(wake);
        }

        return updatedWaitType != WaitType::Cancelled;
    }

    operator bool() const {
        return handle && !IsCancelled();
    }
};

namespace detail
{

Coroutine CoroutinePromise::get_return_object() {
    return Coroutine(coroutine_handle<CoroutinePromise>::from_promise(*this));
}

} // detail

} // scene
} // mw
