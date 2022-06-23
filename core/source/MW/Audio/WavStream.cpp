/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Audio.WavStream;
import Microwave.Audio.AudioSample;
import Microwave.Audio.AudioStream;
import Microwave.IO.Stream;
import Microwave.System.Exception;
import Microwave.System.Pointers;
import <MW/System/Debug.h>;
import <array>;
import <cstddef>;
import <cstdint>;
import <span>;
import <stdexcept>;
import <vector>;
import <string.h>;

namespace mw {
inline namespace audio {

enum class WavFormat : uint16_t
{
    PCM = 1, // integer
    FLOAT = 3
};

struct WavChunkInfo
{
    char type[4];
    uint32_t size;
};

struct WavHeaderChunk
{
    char type[4]; // should be "WAVE"
};

struct WavDS64Chunk
{
    uint64_t riffSize;    // replaces header's WavChunkInfo::size if it's -1
    uint64_t dataSize;    // replaces WavDataChunk::size if it's -1
    uint64_t sampleCount; // replaces WavFactChunk::sampleCount if it's -1 (unused)
};

struct WavJunkChunk
{
    WavDS64Chunk reserved;
};

struct WavFormatChunk
{
    uint16_t audioFormat;    // WavFormat (1 means uncompressed)
    uint16_t numOfChan;      // Number of channels 1-5
    uint32_t samplesPerSec;  // Sampling Frequency in Hz
    uint32_t bytesPerSec;    // bytes per second
    uint16_t blockAlign;     // bytes per frame
    uint16_t bitsPerSample;  // Number of bits per sample
    // extended format info (MS)
};

struct WavPeakChunk
{
    uint32_t version;
    uint32_t timestamp;
    //WavPeakPos peaks[]; // one per channel
};

struct WavDataChunk
{
    //std::byte data[];
};

WavStream::WavStream(const gptr<Stream>& stream)
    : stream(stream)
{
    uint64_t dataSize64 = 0;
    bool foundData = false;

    while (!foundData)
    {
        WavChunkInfo info;
        if (!stream->ReadValue(info))
        {
            throw Exception("cannot read chunk info");
        }

        if (strncmp(info.type, "RIFF", 4) == 0)
        {
            WavHeaderChunk headerChunk;
            if (!stream->ReadValue(headerChunk))
                throw Exception("cannot read RIFF chunk");

            if (strncmp(headerChunk.type, "WAVE", 4) != 0)
                throw Exception("invalid RIFF type");

            // skip padding byte
            stream->Ignore(info.size % 2);
        }
        else if (strncmp(info.type, "RF64", 4) == 0)
        {
            WavHeaderChunk headerChunk;
            if (!stream->ReadValue(headerChunk))
                throw Exception("cannot read RF64 chunk");

            if (strncmp(headerChunk.type, "WAVE", 4) != 0)
                throw Exception("invalid RF64 type");

            // skip padding byte
            stream->Ignore(info.size % 2);
        }
        else if (strncmp(info.type, "ds64", 4) == 0)
        {
            WavDS64Chunk ds64Chunk;
            if (!stream->ReadValue(ds64Chunk))
                throw Exception("cannot read ds64 chunk");

            dataSize64 = ds64Chunk.dataSize;
            stream->Ignore(info.size - sizeof(WavDS64Chunk));

            // skip padding byte
            stream->Ignore(info.size % 2);
        }
        else if (strncmp(info.type, "fmt ", 4) == 0)
        {
            WavFormatChunk formatChunk;
            if (!stream->ReadValue(formatChunk))
                throw Exception("cannot read format chunk");

            numOfChan = formatChunk.numOfChan;
            samplesPerSec = formatChunk.samplesPerSec;
            bytesPerFrame = formatChunk.blockAlign;
            bytesPerSample = bytesPerFrame / numOfChan;

            sampleType = SampleType::Unspecified;

            if (formatChunk.audioFormat == (int)WavFormat::PCM)
            {
                if (formatChunk.bitsPerSample == 8)
                    sampleType = SampleType::Int8;
                else if (formatChunk.bitsPerSample == 16)
                    sampleType = SampleType::Int16;
                else if (formatChunk.bitsPerSample == 24)
                    sampleType = SampleType::Int24;
                else if (formatChunk.bitsPerSample == 32)
                    sampleType = SampleType::Int32;
                else if (formatChunk.bitsPerSample == 64)
                    sampleType = SampleType::Int64;
            }
            else if (formatChunk.audioFormat == (int)WavFormat::FLOAT)
            {
                if (formatChunk.bitsPerSample == 32)
                    sampleType = SampleType::Float32;
                else if (formatChunk.bitsPerSample == 64)
                    sampleType = SampleType::Float64;
            }

            stream->Ignore(info.size - sizeof(WavFormatChunk));

            // skip padding byte
            stream->Ignore(info.size % 2);
        }
        else if (strncmp(info.type, "junk", 4) == 0)
        {
            stream->Ignore(info.size);

            // skip padding byte
            stream->Ignore(info.size % 2);
        }
        else if (strncmp(info.type, "PEAK", 4) == 0)
        {
            WavPeakChunk peakChunk;
            if (!stream->ReadValue(peakChunk))
                throw Exception("failed to read PEAK chunk");

            Assert(sizeof(WavPeakPos) == (info.size - sizeof(WavPeakChunk)) / GetChannels());

            peaks.resize(GetChannels());

            for (int i = 0; i < GetChannels(); ++i)
            {
                if (!stream->ReadValue(peaks[i]))
                    throw Exception("cannot read peak pos");
            }

            size_t bytesRead = sizeof(WavPeakChunk) + sizeof(WavPeakPos) * GetChannels();
            stream->Ignore(info.size - bytesRead);

            // skip padding byte
            stream->Ignore(info.size % 2);
        }
        else if (strncmp(info.type, "data", 4) == 0)
        {
            dataOffset = stream->GetPosition();

            if (info.size == 0xFFFFFFFF)
            {
                if constexpr (sizeof(size_t) >= sizeof(uint64_t))
                    dataSize = dataSize64;
                else
                    throw Exception("RF64 wav only supported in 64 library");
            }
            else
            {
                dataSize = info.size;
            }

            foundData = true;
        }
        else
        {
            // skip unsupported chunks (and padding)
            stream->Ignore(info.size + (info.size % 2));
        }
    }
}

bool WavStream::CanRead() const {
    return stream->CanRead();
}

bool WavStream::CanSeek() const {
    return stream->CanSeek();
}

bool WavStream::CanWrite() const {
    return false;
}

size_t WavStream::GetLength() const {
    return (size_t)dataSize;
}

size_t WavStream::GetPosition() const {
    return stream->GetPosition() - (size_t)dataOffset;
}

size_t WavStream::Seek(int64_t offset, SeekOrigin origin)
{
    int64_t frameOffset = offset / bytesPerFrame;
    int64_t byteOffset = frameOffset * bytesPerFrame;

    int64_t pos = {};

    if (origin == SeekOrigin::Begin)
        pos = dataOffset + byteOffset;
    else if (origin == SeekOrigin::Current)
        pos = GetPosition() + byteOffset;
    else if (origin == SeekOrigin::End)
        pos = dataSize + byteOffset;

    if (pos < (int64_t)dataOffset)
        throw Exception("seek position out of range");

    return stream->Seek(pos, SeekOrigin::Begin);
}

void WavStream::SetLength(size_t length) {
    throw Exception("not implemented");
}

int WavStream::Read(std::span<std::byte> output)
{
    auto requestedFrames = output.size() / bytesPerFrame;
    auto requestedBytes = requestedFrames * bytesPerFrame;
        
    int totalBytesRead = 0;
    
    size_t frameBytesRead = 0;
    std::array<std::byte, 64> buff; // enough to hold 8x64bit samples

    while (requestedFrames != 0)
    {
        auto frameBuff = std::span<std::byte>(
            buff.data() + frameBytesRead,
            bytesPerFrame - frameBytesRead);

        size_t bytesRead = stream->Read(frameBuff);
        if (bytesRead == 0)
            break; // EOF

        frameBytesRead += bytesRead;

        if (frameBytesRead == bytesPerFrame)
        {
            frameBytesRead = 0;
            std::copy(buff.data(), buff.data() + bytesPerFrame, &output[totalBytesRead]);
            totalBytesRead += bytesPerFrame;
            --requestedFrames;
        }
    }

    return totalBytesRead;
}

void WavStream::Write(std::span<std::byte> buffer) {
    throw Exception("not implemented");
}

void WavStream::Flush() {
    stream->Flush();
}

void WavStream::Close() {
    stream->Close();
}

SampleType WavStream::GetSampleType() const {
    return sampleType;
}

int WavStream::GetSampleRate() const {
    return samplesPerSec;
}

int WavStream::GetChannels() const {
    return numOfChan;
}

int WavStream::GetBytesPerSample() const {
    return bytesPerSample;
}

int WavStream::GetFrameCount() const {
    return (int)(dataSize / bytesPerFrame);
}

const std::vector<WavPeakPos>& WavStream::GetPeaks() const {
    return peaks;
}

void WavStream::Save(const gptr<AudioStream>& stream, const gptr<Stream>& output)
{
    if (!stream)
        throw Exception("'stream' cannot be null");

    if (!output)
        throw Exception("'output' cannot be null");

    uint32_t size = 0;

    size += sizeof(WavChunkInfo);
    size += sizeof(WavHeaderChunk);

    size += sizeof(WavChunkInfo);
    size += sizeof(WavFormatChunk);

    size += sizeof(WavChunkInfo);
    size += stream->GetLength(); // WavDataChunk

    WavChunkInfo headerChunkInfo = {
        { 'R', 'I', 'F', 'F' },
        size
    };

    WavHeaderChunk headerChunk = {
        { 'W', 'A', 'V', 'E' }
    };

    output->WriteValue(headerChunkInfo);
    output->WriteValue(headerChunk);

    // FORMAT
    WavChunkInfo formatChunkInfo = {
        { 'f', 'm', 't', ' ' },
        sizeof(WavFormatChunk)
    };

    WavFormatChunk formatChunk;
    formatChunk.numOfChan = (uint16_t)stream->GetChannels();
    formatChunk.samplesPerSec = (uint32_t)stream->GetSampleRate();
    formatChunk.bytesPerSec = (uint32_t)(stream->GetBytesPerSample() * stream->GetSampleRate());
    formatChunk.blockAlign = (uint16_t)(stream->GetBytesPerSample() * stream->GetChannels());
    formatChunk.bitsPerSample = (uint16_t)(stream->GetBytesPerSample() * 8);

    switch (stream->GetSampleType())
    {
    case SampleType::Int8:
        formatChunk.audioFormat = (int)WavFormat::PCM;
        formatChunk.bitsPerSample = 8;
        break;
    case SampleType::Int16:
        formatChunk.audioFormat = (int)WavFormat::PCM;
        formatChunk.bitsPerSample = 16;
        break;
    case SampleType::Int24:
        formatChunk.audioFormat = (int)WavFormat::PCM;
        formatChunk.bitsPerSample = 24;
        break;
    case SampleType::Int32:
        formatChunk.audioFormat = (int)WavFormat::PCM;
        formatChunk.bitsPerSample = 32;
        break;
    case SampleType::Int64:
        formatChunk.audioFormat = (int)WavFormat::PCM;
        formatChunk.bitsPerSample = 64;
        break;
    case SampleType::Float32:
        formatChunk.audioFormat = (int)WavFormat::FLOAT;
        formatChunk.bitsPerSample = 32;
        break;
    case SampleType::Float64:
        formatChunk.audioFormat = (int)WavFormat::FLOAT;
        formatChunk.bitsPerSample = 64;
        break;
    }

    output->WriteValue(formatChunkInfo);
    output->WriteValue(formatChunk);

    // PEAK
    //WavChunkInfo peakChunkInfo = {
    //    { 'P', 'E', 'A', 'K' },
    //    sizeof(WavPeakChunk) + sizeof(WavPeakPos) * GetChannels()
    //};

    //WavPeakChunk peakChunk = {
    //    1,
    //    (uint32_t)time(NULL)
    //};

    //stream->WriteValue(peakChunkInfo);
    //stream->WriteValue(peakChunk);

    //int frameCount = GetFrameCount();

    //peaks.resize(numOfChan, WavPeakPos{ 0, 0 });

    //for (int f = 0; f < frameCount; ++f)
    //{
    //    for (int c = 0; c < numOfChan; ++c)
    //    {
    //        float value = GetSample(f, c);

    //        if (abs(value) > abs(peaks[c].value))
    //        {
    //            peaks[c].value = value;
    //            peaks[c].pos = f;
    //        }
    //    }
    //}

    //for (int c = 0; c < numOfChan; ++c)
    //    stream->WriteValue(peaks[c]);

    size_t dataSize = stream->GetLength();

    std::vector<std::byte> data;
    data.resize(dataSize);

    if (stream->Read(data) != dataSize)
        throw Exception("failed to read data");

    // DATA
    WavChunkInfo dataChunkInfo = {
        { 'd', 'a', 't', 'a' },
        (uint32_t)dataSize
    };

    output->WriteValue(dataChunkInfo);
    output->Write(data);
    
    if (dataChunkInfo.size % 2 != 0)
        output->WriteValue<char>(0);
}

} // audio
} // mw
