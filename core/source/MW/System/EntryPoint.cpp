/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/System/Platforms.h>
#include <MW/System/App.h>
#include <cstdlib>

#if PLATFORM_WINDOWS

#include <Windows.h>

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    return mw::App::Run(__argc, __argv);
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

#elif PLATFORM_WINDOWS_PHONE

[Platform::MTAThread]
int main(Platform::xarray<Platform::String^>^ args)
{
    // cool story bro
    return -1;
}
#endif
