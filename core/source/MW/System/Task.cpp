/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/System/Task.h>
#include <MW/System/ThreadPool.h>
#include <thread>
#include <Windows.h>

namespace mw {
inline namespace system {

namespace detail {

Task<void> GetDelayTask(milliseconds length)
{
    return ThreadPool::InvokeAsync([len = length]{
        std::this_thread::sleep_for(len);
    });
}

} // detail

} // system
} // mw
