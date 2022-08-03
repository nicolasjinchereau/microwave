/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Tools.DynamicLibrary;
import <cstddef>;
import <cstdint>;
import <string>;

export namespace mw {
inline namespace tools {

class DynamicLibrary
{
    void* dylib = nullptr;

public:
    DynamicLibrary(const std::string& filename);
    ~DynamicLibrary();

    template<class Func>
    bool GetFunction(const std::string& name, Func& out_Fun) {
        out_Fun = (Func)GetFunction(name);
        return out_Fun != nullptr;
    }

private:
    void* GetFunction(const std::string& name);
};

} // tools
} // mw
