/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Audio.Mp3Stream;
import Microwave.Audio.Internal.Mp3Decoder;
import Microwave.Audio.AudioSample;
import Microwave.IO.Stream;
import Microwave.System.Exception;
import Microwave.System.Pointers;
import <cstddef>;
import <cstdint>;
import <stdexcept>;
import <span>;
import <algorithm>;

namespace mw {
inline namespace audio {

Mp3Stream::Mp3Stream(const gptr<Stream>& stream)
    : stream(stream), decoder(upnew<Mp3Decoder>(stream.get()))
{
    sampleType = SampleType::Int16;
    samplesPerSec = decoder->sampleRate();
    numOfChan = decoder->channels();
    bytesPerSample = audio::GetBytesPerSample(sampleType);
    bytesPerFrame = bytesPerSample * numOfChan;
    frameCount = (int)decoder->pcmFrameCount();
    dataSize = frameCount * bytesPerFrame;
}

Mp3Stream::~Mp3Stream() {
    
}

bool Mp3Stream::CanRead() const {
    return stream->CanRead();
}

bool Mp3Stream::CanSeek() const {
    return stream->CanSeek();
}

bool Mp3Stream::CanWrite() const {
    return false;
}

size_t Mp3Stream::GetLength() const {
    return (size_t)dataSize;
}

size_t Mp3Stream::GetPosition() const {
    return (size_t)(framePos * bytesPerFrame);
}

size_t Mp3Stream::Seek(int64_t offset, SeekOrigin origin)
{
    if (!CanSeek())
        throw Exception("stream is not seekable");

    int64_t frameOffset = offset / bytesPerFrame;

    int64_t pos = {};

    if (origin == SeekOrigin::Begin)
        pos = frameOffset;
    else if (origin == SeekOrigin::Current)
        pos = framePos + frameOffset;
    else if (origin == SeekOrigin::End)
        pos = frameCount + frameOffset;

    auto ret = decoder->SeekToPCMFrame((uint64_t)pos);
    if(!ret)
        throw Exception("seek failed");

    framePos = pos;

    return (size_t)(framePos * bytesPerFrame);
}

void Mp3Stream::SetLength(size_t length) {
    throw Exception("not implemented");
}

int Mp3Stream::Read(std::span<std::byte> output)
{
    int requestedFrames = output.size() / bytesPerFrame;
    int requestedBytes = requestedFrames * bytesPerFrame;

    int totalBytesRead = 0;

    while (requestedFrames != 0)
    {
        std::array<std::byte, 8 * sizeof(int16_t)> buffer{};
        
        auto framesRead = decoder->ReadPCMFrames(1, (int16_t*)buffer.data());
        if (framesRead == 0)
            break; // EOF

        int samplesRead = (int)framesRead * numOfChan;

        std::copy(buffer.data(), buffer.data() + bytesPerFrame, &output[totalBytesRead]);
        totalBytesRead += bytesPerFrame;
        --requestedFrames;
    }

    return totalBytesRead;
}

void Mp3Stream::Write(std::span<std::byte> buffer) {
    throw Exception("stream is not writable");
}

void Mp3Stream::Flush() {

}

void Mp3Stream::Close() {
    //stream->Close();
}

// `AudioStream` interface
SampleType Mp3Stream::GetSampleType() const {
    return sampleType;
}

int Mp3Stream::GetSampleRate() const {
    return samplesPerSec;
}

int Mp3Stream::GetChannels() const {
    return numOfChan;
}

int Mp3Stream::GetBytesPerSample() const {
    return bytesPerSample;
}

int Mp3Stream::GetFrameCount() const {
    return frameCount;
}

} // audio
} // mw
