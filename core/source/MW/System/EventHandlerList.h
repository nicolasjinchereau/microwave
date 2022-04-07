/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <vector>
#include <MW/System/Pointers.h>

namespace mw {
inline namespace system {

template<class Handler>
class EventHandlerList
{
    std::vector<wptr<Handler>> handlers;
    std::vector<sptr<Handler>> cache;
public:

    void Add(const sptr<Handler>& handler)
    {
        Flush();
        handlers.push_back(handler);
    }

    void Remove(const sptr<Handler>& handler)
    {
        for (auto it = handlers.begin();
            it != handlers.end(); )
        {
            auto h = it->lock();
            if (!h || h == handler) {
                it = handlers.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    template<class Fun, class... Args>
    void RaiseEvent(Fun fun, Args&&... args)
    {
        for (auto& wp : handlers)
        {
            if (auto h = wp.lock())
                cache.push_back(h);
        }

        for (auto& h : cache)
        {
            try {
                ((*h).*fun)(std::forward<Args>(args)...);
            }
            catch (std::exception& ex) {
                Console::WriteLine(ex.what());
            }
        }

        cache.clear();
    }

    template<class Lock, class Fun, class... Args>
    void RaiseEvent(Lock& lock, Fun fun, Args&&... args)
    {
        lock.lock();
        for (auto& wp : handlers)
        {
            if (auto h = wp.lock())
                cache.push_back(h);
        }
        lock.unlock();

        for (auto& h : cache)
        {
            try {
                ((*h).*fun)(std::forward<Args>(args)...);
            }
            catch (std::exception& ex) {
                Console::WriteLine("event handler invocation falied: %", ex.what());
            }
        }

        cache.clear();
    }

private:
    void Flush()
    {
        for (auto it = handlers.begin();
            it != handlers.end(); )
        {
            if (it->lock()) {
                ++it;
            }
            else {
                it = handlers.erase(it);
            }
        }
    }
};

} // system
} // mw
