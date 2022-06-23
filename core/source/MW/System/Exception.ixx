/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.System.Exception;
import <MW/System/Debug.h>;
import <cstdio>;
import <cstdlib>;
import <exception>;
import <source_location>;
import <sstream>;
import <string>;

export namespace mw {
inline namespace system {

struct ExceptionMessage
{
    std::string value;

    ExceptionMessage(const char* message)
        : value(message)
    {
    }

    template<class... Args>
    ExceptionMessage(Args&&... args)
    {
        std::stringstream ss;
        ((ss << std::forward<Args>(args)), ...) << '\n';
        value = std::move(ss).str();
    }
};

class Exception : public std::exception
{
    //std::source_location loc;
    std::string msg;
public:
    Exception(ExceptionMessage msg)//, std::source_location loc = std::source_location::current())
        : /* loc(loc), */ msg(std::move(msg.value))
    {
#if MW_DEBUG
        Print();
#endif
    }

    Exception()//std::source_location loc = std::source_location::current())
        : /* loc(loc), */ msg("<empty>")
    {
#if MW_DEBUG
        Print();
#endif
    }

    const std::string& GetMessage() const {
        return msg;
    }

    //const std::source_location& GetLocation() const {
    //    return loc;
    //}

    void Print()
    {
        printf("Exception has occured:\n %s\n", msg.c_str());
        //printf(
        //    "Exception has occured:\n file: %s\n line: %d\n function: %s\n message: %s\n",
        //    loc.file_name(), (int)loc.line(), loc.function_name(), msg.c_str());
    }

    virtual char const* what() const override {
        return msg.c_str();
    }
};

} // system
} // mw
