/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.System.IAwaitable;
import Microwave.System.Pointers;
import Microwave.System.IAwaiter;

export namespace mw {
inline namespace system {

template<class T>
class IAwaitable
{
public:
    virtual ~IAwaitable() = default;
    virtual bool IsAwaitableReady() = 0;
    virtual void OnAwaiterSuspended(const gptr<IAwaiter>& awaiter) = 0;
    virtual T OnAwaiterResumed() = 0;
    virtual T WaitForResult() = 0;
};

} // system
} // mw
