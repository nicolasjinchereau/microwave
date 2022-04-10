/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.System.Task;
import Microwave.System.ThreadPool;
import <thread>;

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
