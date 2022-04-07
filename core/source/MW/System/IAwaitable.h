/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/System/Pointers.h>
#include <MW/System/IAwaiter.h>

namespace mw {
inline namespace system {

template<class T>
class IAwaitable
{
public:
    virtual ~IAwaitable() = default;
    virtual bool IsAwaitableReady() = 0;
    virtual void OnAwaiterSuspended(const sptr<IAwaiter>& caller) = 0;
    virtual T OnAwaiterResumed() = 0;
    virtual T WaitForResult() = 0;
};

} // system
} // mw
