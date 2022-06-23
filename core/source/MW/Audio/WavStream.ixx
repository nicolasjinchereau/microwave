/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Audio.WavStream;
import Microwave.Audio.AudioSample;
import Microwave.Audio.AudioStream;
import Microwave.IO.Stream;
import Microwave.System.Pointers;
import <cstddef>;
import <cstdint>;
import <span>;
import <vector>;

export namespace mw {
inline namespace audio {

struct WavPeakPos
{
    float value = {};
    uint32_t pos = {};
};

class WavStream : public AudioStream
{
    gptr<Stream> stream;

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
    WavStream(const gptr<Stream>& stream);

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

    static void Save(const gptr<AudioStream>& stream, const gptr<Stream>& output);
};

} // audio
} // mw
