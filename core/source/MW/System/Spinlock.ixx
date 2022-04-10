/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.System.Spinlock;
import <atomic>;

export namespace mw {
inline namespace system {

class Spinlock
{
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
public:
    Spinlock() = default;

    Spinlock(const Spinlock&) = delete;
    Spinlock& operator=(const Spinlock&) = delete;

    Spinlock(Spinlock&&) = delete;
    Spinlock& operator=(Spinlock&&) = delete;

    void lock() {
        while(flag.test_and_set(std::memory_order_acquire)){}
    }

    void unlock() {
        flag.clear(std::memory_order_release);
    }
};

} // system
} // mw
