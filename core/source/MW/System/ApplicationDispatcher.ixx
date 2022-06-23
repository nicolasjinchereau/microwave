/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.System.ApplicationDispatcher;
import Microwave.System.Dispatcher;
import Microwave.System.Exception;
import Microwave.System.Pointers;
import <stdexcept>;

export namespace mw {
inline namespace system {

class App;

class ApplicationDispatcher : public Dispatcher
{
    static gptr<ApplicationDispatcher> New();
    friend App;

public:
    virtual void Run() final override { throw Exception("Use `Run(int, char*[])` instead of Dispatcher::Run."); }
    virtual void Run(int argc, char* argv[]) = 0;
};

} // system
} // mw
