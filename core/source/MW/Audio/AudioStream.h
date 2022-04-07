/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Audio/AudioSample.h>
#include <MW/IO/Stream.h>
#include <MW/System/Pointers.h>
#include <cstdint>

namespace mw {
inline namespace audio {

class AudioStream : public Stream
{
public:
    virtual SampleType GetSampleType() const = 0;
    virtual int GetSampleRate() const = 0;
    virtual int GetChannels() const = 0;
    virtual int GetBytesPerSample() const = 0;
    virtual int GetFrameCount() const = 0;
};

}
}
