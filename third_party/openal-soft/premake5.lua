require "android-lib"

workspace "openal-soft"
    configurations { "Debug", "Release" }
    location ("projects/" .. os.target())

project "openal-soft"
    kind "StaticLib"
    language "C"
    
    location ("projects/" .. os.target())
    targetname "openal-soft"

    files {
        "source/common/alcomplex.cpp",
        "source/common/alfstream.cpp",
        "source/common/almalloc.cpp",
        "source/common/alstring.cpp",
        "source/common/dynload.cpp",
        "source/common/polyphase_resampler.cpp",
        "source/common/ringbuffer.cpp",
        "source/common/strutils.cpp",
        "source/common/threads.cpp",
        "source/al/auxeffectslot.cpp",
        "source/common/albit.h",
        "source/common/albyte.h",
        "source/common/alcomplex.h",
        "source/common/aldeque.h",
        "source/common/alfstream.h",
        "source/common/almalloc.h",
        "source/common/alnumeric.h",
        "source/common/aloptional.h",
        "source/common/alspan.h",
        "source/common/alstring.h",
        "source/common/atomic.h",
        "source/common/dynload.h",
        "source/common/intrusive_ptr.h",
        "source/common/math_defs.h",
        "source/common/opthelpers.h",
        "source/common/polyphase_resampler.h",
        "source/common/pragmadefs.h",
        "source/common/ringbuffer.h",
        "source/common/strutils.h",
        "source/common/threads.h",
        "source/common/vecmat.h",
        "source/common/vector.h",
        "source/al/auxeffectslot.h",
        "source/al/buffer.cpp",
        "source/al/buffer.h",
        "source/al/effect.cpp",
        "source/al/effect.h",
        "source/al/effects/autowah.cpp",
        "source/al/effects/chorus.cpp",
        "source/al/effects/compressor.cpp",
        "source/al/effects/convolution.cpp",
        "source/al/effects/dedicated.cpp",
        "source/al/effects/distortion.cpp",
        "source/al/effects/echo.cpp",
        "source/al/effects/effects.h",
        "source/al/effects/equalizer.cpp",
        "source/al/effects/fshifter.cpp",
        "source/al/effects/modulator.cpp",
        "source/al/effects/null.cpp",
        "source/al/effects/pshifter.cpp",
        "source/al/effects/reverb.cpp",
        "source/al/effects/vmorpher.cpp",
        "source/al/error.cpp",
        "source/al/event.cpp",
        "source/al/event.h",
        "source/al/extension.cpp",
        "source/al/filter.cpp",
        "source/al/filter.h",
        "source/al/listener.cpp",
        "source/al/listener.h",
        "source/al/source.cpp",
        "source/al/source.h",
        "source/al/state.cpp",
        "source/alc/alc.cpp",
        "source/alc/alcmain.h",
        "source/alc/alu.cpp",
        "source/alc/alu.h",
        "source/alc/alconfig.cpp",
        "source/alc/alconfig.h",
        "source/alc/alcontext.h",
        "source/alc/async_event.h",
        "source/alc/bformatdec.cpp",
        "source/alc/bformatdec.h",
        "source/alc/buffer_storage.cpp",
        "source/alc/buffer_storage.h",
        "source/alc/compat.h",
        "source/alc/converter.cpp",
        "source/alc/converter.h",
        "source/alc/effectslot.cpp",
        "source/alc/effectslot.h",
        "source/alc/effects/base.h",
        "source/alc/effects/autowah.cpp",
        "source/alc/effects/chorus.cpp",
        "source/alc/effects/compressor.cpp",
        "source/alc/effects/convolution.cpp",
        "source/alc/effects/dedicated.cpp",
        "source/alc/effects/distortion.cpp",
        "source/alc/effects/echo.cpp",
        "source/alc/effects/equalizer.cpp",
        "source/alc/effects/fshifter.cpp",
        "source/alc/effects/modulator.cpp",
        "source/alc/effects/null.cpp",
        "source/alc/effects/pshifter.cpp",
        "source/alc/effects/reverb.cpp",
        "source/alc/effects/vmorpher.cpp",
        "source/alc/front_stablizer.h",
        "source/alc/helpers.cpp",
        "source/alc/hrtf.cpp",
        "source/alc/hrtf.h",
        "source/alc/inprogext.h",
        "source/alc/panning.cpp",
        "source/alc/uiddefs.cpp",
        "source/alc/voice.cpp",
        "source/alc/voice.h",
        "source/alc/voice_change.h",
        "source/alc/backends/base.cpp",
        "source/alc/backends/base.h",
        "source/alc/backends/loopback.cpp",
        "source/alc/backends/loopback.h",
        "source/alc/backends/null.cpp",
        "source/alc/backends/null.h",
        "source/alc/backends/winmm.cpp",
        "source/alc/backends/winmm.h",
        "source/alc/backends/dsound.cpp",
        "source/alc/backends/dsound.h",
        "source/alc/backends/wasapi.cpp",
        "source/alc/backends/wasapi.h",
        "source/alc/backends/wave.cpp",
        "source/alc/backends/wave.h",
        "source/config.h",
        "source/hrtf_default.h",
        "source/core/ambdec.cpp",
        "source/core/ambdec.h",
        "source/core/ambidefs.h",
        "source/core/bs2b.cpp",
        "source/core/bs2b.h",
        "source/core/bsinc_defs.h",
        "source/core/bsinc_tables.cpp",
        "source/core/bsinc_tables.h",
        "source/core/bufferline.h",
        "source/core/cpu_caps.cpp",
        "source/core/cpu_caps.h",
        "source/core/devformat.cpp",
        "source/core/devformat.h",
        "source/core/except.cpp",
        "source/core/except.h",
        "source/core/filters/biquad.h",
        "source/core/filters/biquad.cpp",
        "source/core/filters/nfc.cpp",
        "source/core/filters/nfc.h",
        "source/core/filters/splitter.cpp",
        "source/core/filters/splitter.h",
        "source/core/fmt_traits.cpp",
        "source/core/fmt_traits.h",
        "source/core/fpu_ctrl.cpp",
        "source/core/fpu_ctrl.h",
        "source/core/logging.cpp",
        "source/core/logging.h",
        "source/core/mastering.cpp",
        "source/core/mastering.h",
        "source/core/uhjfilter.cpp",
        "source/core/uhjfilter.h",
        "source/core/mixer/defs.h",
        "source/core/mixer/hrtfbase.h",
        "source/core/mixer/hrtfdefs.h",
        "source/core/mixer/mixer_c.cpp",
        "source/core/mixer/mixer_sse.cpp",
        "source/core/mixer/mixer_sse2.cpp",
        "source/core/mixer/mixer_sse3.cpp",
        "source/core/mixer/mixer_sse41.cpp",
        "source/version.h",
        "include/AL/al.h",
        "include/AL/alc.h",
        "include/AL/alext.h",
        "include/AL/efx.h"
    }

    sysincludedirs {
        "include",
        "source",
        "source/al",
        "source/alc",
        "source/common"
    }

    androidabis {
		"armeabi-v7a", "arm64-v8a"
    }

    androidminsdkversion "android-16"
    
    filter { "action:vs*" }
        objdir ("!obj/$(PlatformName)/$(Configuration)")
        targetdir ("lib/$(PlatformName)/$(Configuration)")
        characterset "MBCS"

    filter { "action:xcode*" }
        objdir ("!obj/$(PLATFORM_NAME)/$(CONFIGURATION)")
        targetdir ("lib/$(PLATFORM_NAME)/$(CONFIGURATION)")
    
    filter { "action:android*" }
        objdir ("obj/android/$(TARGET_ARCH_ABI)/$(APP_OPTIM)")
        targetdir ("lib/android/$(TARGET_ARCH_ABI)/$(APP_OPTIM)")

    filter {}
        defines {
            "WIN32",
            "_WINDOWS",
            "AL_BUILD_LIBRARY",
            "AL_ALEXT_PROTOTYPES",
            "_WIN32",
            "RESTRICT=__restrict",
            "_CRT_SECURE_NO_WARNINGS",
            "NOMINMAX",
            "OpenAL_EXPORTS"
        }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "Speed"
        inlining "Auto"
        vectorextensions "SSE2"

    filter { "action:xcode*" }
        xcodebuildsettings {           
            ["CLANG_CXX_LANGUAGE_STANDARD"] = "gnu++20";
            ["GCC_C_LANGUAGE_STANDARD"] = "gnu11";
            ["CLANG_CXX_LIBRARY"] = "libc++";
        }
    
    filter { "action:vs*" }
        cppdialect "C++20"
        cdialect "C11"
        disablewarnings { "4065", "4268", "4324", "5030" }
        
        buildoptions {
            "/w14640"
        }