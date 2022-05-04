/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

import Microwave.System.App;
import <cstdlib>;
#include <MW/System/Internal/Platform.h>

#if PLATFORM_WINDOWS

int main(int argc, char* argv[]) {
    return mw::App::Run(argc, argv);
}

#elif PLATFORM_MACOS

int main(int argc, char *argv[]) {
    return mw::App::Run(argc, argv);
}

#elif PLATFORM_IOS

int main(int argc, char *argv[]) {
    return mw::App::Run(argc, argv);
}

#elif PLATFORM_ANDROID

#include <android_native_app_glue.h>

namespace mw {
    android_app* androidApp = nullptr;
}

__attribute__((used))
__attribute__((visibility ("default")))
extern "C" void android_main(struct android_app* app) {
    mw::androidApp = app;
    mw::App::Run(0, nullptr);
    mw::androidApp = nullptr;
}

#endif
