/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.System.EventHandlerList;
import Microwave.IO.Terminal;
import Microwave.System.Exception;
import Microwave.System.Pointers;
import <utility>;
import <vector>;

export namespace mw {
inline namespace system {

template<class Handler>
class EventHandlerList
{
    gvector<wgptr<Handler>> handlers;
    gvector<gptr<Handler>> cache;
public:

    void Add(const gptr<Handler>& handler)
    {
        Flush();
        handlers.push_back(handler);
    }

    void Remove(const gptr<Handler>& handler)
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
            catch (const Exception& ex) {
                writeln(ex.what());
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
            catch (const Exception& ex) {
                writeln("event handler invocation falied: ", ex.what());
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
