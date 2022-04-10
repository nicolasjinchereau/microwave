/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Audio.AudioStream;
import Microwave.Audio.AudioSample;
import Microwave.IO.Stream;
import Microwave.System.Object;
import <cstdint>;

export namespace mw {
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

} // audio
} // mw
