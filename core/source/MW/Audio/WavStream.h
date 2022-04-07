/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Audio/AudioStream.h>
#include <MW/Audio/AudioSample.h>
#include <MW/IO/Stream.h>
#include <MW/System/Pointers.h>
#include <cstdint>

namespace mw {
inline namespace audio {

struct WavPeakPos
{
    float value;
    uint32_t pos;
};

class WavStream : public AudioStream
{
    sptr<Stream> stream;

    SampleType sampleType = {};
    int samplesPerSec = {}; // frames per second
    int numOfChan = {}; // channel count
    int bytesPerFrame = {};
    int bytesPerSample = {};

    uint64_t dataSize = {};
    uint64_t dataOffset = {};

    std::vector<WavPeakPos> peaks; // one per channel
public:
    // view of a *.wav file as a stream of samples
    WavStream(const sptr<Stream>& stream);

    // `Stream` interface
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
    virtual SampleType GetSampleType() const override;
    virtual int GetSampleRate() const override;
    virtual int GetChannels() const override;
    virtual int GetBytesPerSample() const override;
    virtual int GetFrameCount() const override;

    // extra
    const std::vector<WavPeakPos> &GetPeaks() const;

    static void Save(const sptr<AudioStream>& stream, const sptr<Stream>& output);
};

} // audio
} // mw
