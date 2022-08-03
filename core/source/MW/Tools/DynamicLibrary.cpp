/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Tools.DynamicLibrary;
import Microwave.System.Exception;
import <MW/System/Internal/PlatformHeaders.h>;

namespace mw {
inline namespace tools {

void* DynamicLibrary::GetFunction(const std::string& name)
{
#if defined(_WIN32)
    return (void*)GetProcAddress((HMODULE)dylib, name.c_str());
#else
    return (void*)dlsym(dylib, name.c_str());
#endif
}

DynamicLibrary::DynamicLibrary(const std::string& filename)
{
#if defined(_WIN32)
    dylib = (void*)LoadLibrary(filename.c_str());
#else
    dylib = (void*)dlopen(filename.c_str(), RTLD_LAZY);
#endif

    if(!dylib)
        throw Exception("failed to load dynmaic library");
}

DynamicLibrary::~DynamicLibrary()
{
#if defined(_WIN32)
    FreeLibrary((HMODULE)dylib);
#else
    dlclose(dylib);
#endif
}

} // tools
} // mw
