/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include <MW/System/Task.h>
#include <MW/System/Awaitable.h>
#include <MW/System/Object.h>

namespace mw {
inline namespace system {

class Executor : public Object
{
protected:
    virtual void Execute(const std::function<void()>& job) = 0;
public:
    virtual ~Executor() = default;

    template<class Fun, class T = std::invoke_result_t<Fun>>
    Task<T> Invoke(Fun&& fun)
    {
        auto awaiter = spnew<Awaitable<T>>();

        std::function<void()> job = [f = std::forward<Fun>(fun), aw = awaiter]() mutable
        {
            try
            {
                if constexpr (std::is_void_v<T>) {
                    f();
                    aw->SetCompleted();
                }
                else {
                    aw->SetCompleted(f());
                }
            }
            catch (...) {
                aw->SetException(std::current_exception());
            }
        };

        Execute(job);

        return Task<T>(awaiter);
    }
};

} // system
} // mw
