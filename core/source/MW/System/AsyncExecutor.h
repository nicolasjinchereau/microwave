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
#include <deque>
#include <MW/System/Dispatcher.h>
#include <MW/System/Executor.h>

namespace mw {
inline namespace system {

class AsyncExecutor : public Executor
{
    REGISTER_OBJECT(AsyncExecutor);

    std::deque<std::function<void()>> jobs;
    std::vector<std::thread> threads;
    std::condition_variable cond;
    std::mutex mut;
    std::atomic<bool> run;

    void DoWork();

public:
    AsyncExecutor(int workerCount = 0);
    ~AsyncExecutor();

protected:
    virtual void Execute(const std::function<void()>& job) override
    {
        {
            std::unique_lock<std::mutex> lk(mut);
            jobs.push_back(job);
        }

        cond.notify_one();
    }
};

} // system
} // mw
