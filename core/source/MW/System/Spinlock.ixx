/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.System.Spinlock;
import <atomic>;
import <cstdint>;
import <thread>;

template<int size = 0> struct ThreadIDType;
template<> struct ThreadIDType<1> { using type = uint8_t; };
template<> struct ThreadIDType<2> { using type = uint16_t; };
template<> struct ThreadIDType<4> { using type = uint32_t; };
template<> struct ThreadIDType<8> { using type = uint64_t; };

export namespace mw {
inline namespace system {

class Spinlock
{
    std::atomic<bool> flag;
public:
    Spinlock()
        : flag(false){}

    Spinlock(const Spinlock&) = delete;
    Spinlock& operator=(const Spinlock&) = delete;

    Spinlock(Spinlock&&) = delete;
    Spinlock& operator=(Spinlock&&) = delete;

    void lock()
    {
        while (flag.exchange(true, std::memory_order_acquire))
        {
            while(flag.load(std::memory_order_relaxed))
            {
            }
        }
    }

    void unlock() {
        flag.store(false, std::memory_order_release);
    }
};

class RecursiveSpinlock
{
    using StdThreadID = decltype(std::this_thread::get_id());
    using ThreadID = ThreadIDType<sizeof(StdThreadID)>::type;

    static ThreadID GetThreadID() {
        auto id = std::this_thread::get_id();
        return *reinterpret_cast<ThreadID*>(&id);
    }

    std::atomic<ThreadID> owner = 0;
    size_t lockCount = 0;
public:
    RecursiveSpinlock() {}

    RecursiveSpinlock(const RecursiveSpinlock&) = delete;
    RecursiveSpinlock(RecursiveSpinlock&&) = delete;
    RecursiveSpinlock& operator=(const RecursiveSpinlock&) = delete;
    RecursiveSpinlock& operator=(RecursiveSpinlock&&) = delete;

    void lock()
    {
        ThreadID currentThreadId = GetThreadID();

        if (owner.load(std::memory_order_relaxed) == currentThreadId) {
            ++lockCount;
            return;
        }

        uint32_t expected = 0;
        while (!owner.compare_exchange_weak(expected, currentThreadId, std::memory_order_acquire))
        {
            while (owner.load(std::memory_order_relaxed) == expected)
            {
            }

            expected = 0;
        }

        lockCount = 1;
    }

    void unlock() {
        if (--lockCount == 0) {
            owner.store(0, std::memory_order_release);
        }
    }
};

} // system
} // mw
