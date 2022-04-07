/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <cstdint>
#include <MW/Audio/AudioSample.h>
#include <MW/Audio/AudioStream.h>

namespace mw {
inline namespace audio {

class OggDecoder;

class OggStream : public AudioStream
{
    sptr<Stream> stream;
    std::unique_ptr<OggDecoder> decoder;

    SampleType sampleType = {};
    int samplesPerSec = {};
    int numOfChan = {};
    int frameCount = {};
    int bytesPerSample = {};
    int bytesPerFrame = {};
    uint64_t dataSize = {};
public:

    // view of an *.ogg file as a stream of samples
    OggStream(const sptr<Stream>& stream);
    ~OggStream();

    virtual bool CanRead() const override;
    virtual bool CanSeek() const override;
    virtual bool CanWrite() const override;
    virtual size_t GetLength() const override;
    virtual size_t GetPosition() const override;
    virtual size_t Seek(int64_t offset, SeekOrigin origin) override;
    virtual void SetLength(size_t length) override;
    virtual int Read(std::span<std::byte> output) override;
    virtual void Write(std::span<std::byte> buffer) override;
    virtual void Flush() override;
    virtual void Close() override;

    // `AudioStream` interface
    SampleType GetSampleType() const;
    int GetSampleRate() const;
    int GetChannels() const;
    int GetBytesPerSample() const;
    int GetFrameCount() const;
};

} // audio
} // mw
