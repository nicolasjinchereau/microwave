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
#include <MW/System/Dispatcher.h>
#include <MW/System/Task.h>
#include <MW/System/AsyncExecutor.h>

namespace mw {
inline namespace system {

class ThreadPool : public AsyncExecutor
{
public:
    template<class Fun, class T = std::invoke_result_t<Fun>>
    static Task<T> InvokeAsync(Fun&& fun) {
        return GetInstance()->Invoke(std::forward<Fun>(fun));
    }

    static const sptr<ThreadPool>& GetInstance() {
        static sptr<ThreadPool> pool = spnew<ThreadPool>();
        return pool;
    }
};

} // system
} // mw
