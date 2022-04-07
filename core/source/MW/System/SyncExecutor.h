/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/System/Executor.h>

namespace mw {
inline namespace system {

class SyncExecutor : public Executor
{
    REGISTER_OBJECT(SyncExecutor);
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
