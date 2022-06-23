/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.System.ThreadPool;
import Microwave.System.AsyncExecutor;
import Microwave.System.Pointers;
import Microwave.System.Task;
import <utility>;
import <type_traits>;

export namespace mw {
inline namespace system {

class ThreadPool : public AsyncExecutor
{
public:
    template<class Fun, class T = std::invoke_result_t<Fun>>
    static Task<T> InvokeAsync(Fun&& fun) {
        return GetInstance()->Invoke(std::forward<Fun>(fun));
    }

    static const gptr<ThreadPool>& GetInstance() {
        static gptr<ThreadPool> pool = gpnew<ThreadPool>();
        return pool;
    }
};

} // system
} // mw
