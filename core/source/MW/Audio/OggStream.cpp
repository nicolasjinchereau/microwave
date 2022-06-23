/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Audio.OggStream;
import Microwave.Audio.AudioSample;
import Microwave.Audio.Internal.OggDecoder;
import Microwave.IO.Stream;
import Microwave.System.Exception;
import Microwave.System.Pointers;
import <cstddef>;
import <cstdint>;
import <span>;
import <stdexcept>;

namespace mw {
inline namespace audio {

OggStream::OggStream(const gptr<Stream>& stream)
    : stream(stream), decoder(new OggDecoder(stream.get()))
{
    sampleType = SampleType::Float32;
    samplesPerSec = (int)decoder->sampleRate();
    numOfChan = decoder->channels();
    bytesPerSample = audio::GetBytesPerSample(sampleType);
    bytesPerFrame = bytesPerSample * numOfChan;
    frameCount = (int)decoder->pcmTotal();
    dataSize = frameCount * bytesPerFrame;
}

OggStream::~OggStream()
{
}

bool OggStream::CanRead() const {
    return stream->CanRead();
}

bool OggStream::CanSeek() const {
    return stream->CanSeek();
}

bool OggStream::CanWrite() const {
    return false;
}

size_t OggStream::GetLength() const {
    auto size = decoder->pcmTotal() * bytesPerFrame;
    return (size_t)size;
}

size_t OggStream::GetPosition() const {
    int64_t pos = decoder->pcmTell() * bytesPerFrame;
    return (size_t)pos;
}

size_t OggStream::Seek(int64_t offset, SeekOrigin origin)
{
    if (!CanSeek())
        throw Exception("stream is not seekable");

    int64_t frameOffset = offset / bytesPerFrame;

    int64_t pos = {};

    if (origin == SeekOrigin::Begin)
        pos = frameOffset;
    else if (origin == SeekOrigin::Current)
        pos = decoder->pcmTell() + frameOffset;
    else if (origin == SeekOrigin::End)
        pos = decoder->pcmTotal() + frameOffset;
    
    int ret = decoder->pcmSeek(pos);
    if (ret != 0)
        throw Exception("seek failed");

    return (size_t)(pos * bytesPerFrame);
}

void OggStream::SetLength(size_t length) {
    throw Exception("not implemented");
}

int OggStream::Read(std::span<std::byte> output)
{
    int requestedFrames = output.size() / bytesPerFrame;
    int requestedBytes = requestedFrames * bytesPerFrame;

    int totalSamplesRead = 0;

    while (requestedFrames != 0)
    {
        float** samples = nullptr; // sample channels
        
        int framesRead = (int)decoder->readFloat(&samples, requestedFrames);
        if (framesRead < 0)
            throw Exception("error reading file");

        if (framesRead == 0)
            break; // EOF

        int samplesRead = framesRead * numOfChan;

        for (int i = 0; i < samplesRead; ++i)
        {
            union {
                std::byte bytes[4];
                float value;
            } sample;

            int chan = i % numOfChan;
            int samp = i / numOfChan;
            sample.value = samples[chan][samp];

            size_t bytesRead = totalSamplesRead * 4;
            std::ranges::copy(sample.bytes, &output[bytesRead]);
            ++totalSamplesRead;
        }

        requestedFrames -= framesRead;
    }

    return totalSamplesRead * bytesPerSample;
}

void OggStream::Write(std::span<std::byte> buffer) {
    throw Exception("stream is not writable");
}

void OggStream::Flush() {

}

void OggStream::Close() {
    //stream->Close();
}

// `AudioStream` interface
SampleType OggStream::GetSampleType() const {
    return sampleType;
}

int OggStream::GetSampleRate() const {
    return samplesPerSec;
}

int OggStream::GetChannels() const {
    return numOfChan;
}

int OggStream::GetBytesPerSample() const {
    return bytesPerSample;
}

int OggStream::GetFrameCount() const {
    return frameCount;
}

} // audio
} // mw
