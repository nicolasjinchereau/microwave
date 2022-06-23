/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.System.SyncExecutor;
import Microwave.System.Executor;
import Microwave.System.Object;
import Microwave.System.Pointers;
import <functional>;

export namespace mw {
inline namespace system {

class SyncExecutor : public Executor
{
    inline static Type::Pin<SyncExecutor> pin;
protected:
    virtual void Execute(const gfunction<void()>& job) override
    {
        job();
    }

public:

    SyncExecutor() {
        SetName("SyncExecutor");
    }

    static const gptr<SyncExecutor>& GetInstance() {
        static gptr<SyncExecutor> executor = gpnew<SyncExecutor>();
        return executor;
    }
};

} // system
} // mw
