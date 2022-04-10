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
    virtual void Execute(const std::function<void()>& job) override
    {
        job();
    }

public:

    SyncExecutor() {
        SetName("SyncExecutor");
    }

    static const sptr<SyncExecutor>& GetInstance() {
        static sptr<SyncExecutor> executor = spnew<SyncExecutor>();
        return executor;
    }
};

} // system
} // mw
