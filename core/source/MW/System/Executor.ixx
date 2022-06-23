/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.System.Executor;
import Microwave.System.Awaitable;
import Microwave.System.Object;
import Microwave.System.Pointers;
import Microwave.System.Task;
import <condition_variable>;
import <exception>;
import <functional>;
import <memory>;
import <mutex>;
import <thread>;
import <type_traits>;
import <vector>;

export namespace mw {
inline namespace system {

class Executor : public Object
{
protected:
    virtual void Execute(const gfunction<void()>& job) = 0;
public:
    virtual ~Executor() = default;

    template<class Fun, class T = std::invoke_result_t<Fun>>
    Task<T> Invoke(Fun&& fun)
    {
        auto awaiter = gpnew<Awaitable<T>>();

        gfunction<void()> job = [f = std::forward<Fun>(fun), aw = awaiter]() mutable
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
