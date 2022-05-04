/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.System.PostExecutor;
import Microwave.System.Dispatcher;
import Microwave.System.Executor;
import Microwave.System.Object;
import Microwave.System.Pointers;
import <functional>;
import <cassert>;

export namespace mw {
inline namespace system {

class PostExecutor : public Executor
{
    inline static Type::Pin<PostExecutor> pin;
    sptr<Dispatcher> dispatcher;
protected:
    virtual void Execute(const std::function<void()>& job) override
    {
        dispatcher->InvokeAsync(job);
    }

public:
    PostExecutor(sptr<Dispatcher> dispatcher)
        : dispatcher(dispatcher)
    {
        assert(dispatcher);
        SetName("PostExecutor");
    }
};

} // system
} // mw
