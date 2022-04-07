/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/System/AsyncExecutor.h>
#include <MW/System/Platforms.h>
#include <cassert>

namespace mw {
inline namespace system {

AsyncExecutor::AsyncExecutor(int workerCount)
{
    SetName("AsyncExecutor");

    if (workerCount == 0)
        workerCount = std::thread::hardware_concurrency();

    run = true;

    for (int i = 0; i < workerCount; ++i)
    {
        std::thread thrd(std::bind(&AsyncExecutor::DoWork, this));
        threads.push_back(std::move(thrd));
    }
}

AsyncExecutor::~AsyncExecutor()
{
    run = false;
    cond.notify_all();

    for (auto& thread : threads)
        thread.join();
}

void AsyncExecutor::DoWork()
{
    while (run)
    {
        std::function<void()> job;
        
        {
            std::unique_lock<std::mutex> lk(mut);
            cond.wait(lk, [this] { return !run || !jobs.empty(); });

            if (!jobs.empty())
            {
                job = std::move(jobs.front());
                jobs.pop_front();
            }
        }

        if (job)
            job();
    }
}

} // system
} // mw
