/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Audio.OggStream;
import Microwave.Audio.AudioSample;
import Microwave.Audio.AudioStream;
import Microwave.IO.Stream;
import Microwave.System.Pointers;
import <cstddef>;
import <cstdint>;
import <span>;

export namespace mw {
inline namespace audio {

class OggDecoder;

class OggStream : public AudioStream
{
    gptr<Stream> stream;
    uptr<OggDecoder> decoder;

    SampleType sampleType = {};
    int samplesPerSec = {};
    int numOfChan = {};
    int frameCount = {};
    int bytesPerSample = {};
    int bytesPerFrame = {};
    uint64_t dataSize = {};
public:

    // view of an *.ogg file as a stream of samples
    OggStream(const gptr<Stream>& stream);
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
