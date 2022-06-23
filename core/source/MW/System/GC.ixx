/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.System.GC;
import Microwave.System.App;
import Microwave.System.Dispatcher;
import Microwave.System.Exception;
import Microwave.System.Pointers;
import Microwave.System.Task;
import Microwave.System.ThreadPool;
import <gc/gc.h>;

export namespace mw {
inline namespace system {

class GC
{
public:
    static Task<void> CollectAsync()
    {
        if (Dispatcher::GetCurrent() != App::Get()->GetDispatcher())
            throw Exception("GC should only be run on main thread");

        gc::garbage g;

        co_await ThreadPool::InvokeAsync([&g]{
            g = gc::graph::collect();
        });

        g = {};
    }
};

} // system
} // mw
