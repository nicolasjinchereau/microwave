/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.System.AsyncExecutor;
import Microwave.System.Dispatcher;
import Microwave.System.Executor;
import Microwave.System.Pointers;
import <atomic>;
import <condition_variable>;
import <deque>;
import <functional>;
import <memory>;
import <mutex>;
import <thread>;

export namespace mw {
inline namespace system {

class AsyncExecutor : public Executor
{
    inline static Type::Pin<AsyncExecutor> pin;

    gvector<gfunction<void()>> jobs;
    std::vector<std::thread> threads;
    std::condition_variable cond;
    std::mutex mut;
    std::atomic<bool> run;

public:
    AsyncExecutor(int workerCount = 0)
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

    ~AsyncExecutor()
    {
        run = false;
        cond.notify_all();

        for (auto& thread : threads)
            thread.join();
    }

protected:
    virtual void Execute(const gfunction<void()>& job) override
    {
        {
            std::unique_lock<std::mutex> lk(mut);
            jobs.push_back(job);
        }

        cond.notify_one();
    }

    void DoWork()
    {
        while (run)
        {
            gfunction<void()> job;

            {
                std::unique_lock<std::mutex> lk(mut);
                cond.wait(lk, [this] { return !run || !jobs.empty(); });

                if (!jobs.empty())
                {
                    job = std::move(jobs.front());
                    jobs.erase(jobs.begin());
                }
            }

            if (job)
                job();
        }
    }
};

} // system
} // mw
