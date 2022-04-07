/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/System/Dispatcher.h>
#include <stdexcept>

namespace mw {
inline namespace system {

class App;

class ApplicationDispatcher : public Dispatcher
{
    static sptr<ApplicationDispatcher> New();
    friend class App;

public:
    virtual void Run() final override { throw std::runtime_error("Use `Run(int, char*[])` instead of Dispatcher::Run."); }
    virtual void Run(int argc, char* argv[]) = 0;
};

} // system
} // mw
