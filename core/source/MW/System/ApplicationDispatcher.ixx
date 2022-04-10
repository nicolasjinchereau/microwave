/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.System.ApplicationDispatcher;
import Microwave.System.Dispatcher;
import Microwave.System.Pointers;
import <stdexcept>;

export namespace mw {
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
