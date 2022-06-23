/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.System.PostExecutor;
import Microwave.System.Dispatcher;
import Microwave.System.Exception;
import Microwave.System.Executor;
import Microwave.System.Object;
import Microwave.System.Pointers;
import <MW/System/Debug.h>;
import <functional>;

export namespace mw {
inline namespace system {

class PostExecutor : public Executor
{
    inline static Type::Pin<PostExecutor> pin;
    gptr<Dispatcher> dispatcher;
protected:
    virtual void Execute(const gfunction<void()>& job) override
    {
        dispatcher->InvokeAsync(job);
    }

public:
    PostExecutor(gptr<Dispatcher> dispatcher)
        : dispatcher(dispatcher)
    {
        Assert(dispatcher);
        SetName("PostExecutor");
    }
};

} // system
} // mw
