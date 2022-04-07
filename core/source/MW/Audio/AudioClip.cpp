/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/Audio/AudioClip.h>
#include <MW/Audio/Mp3Stream.h>
#include <MW/Audio/OggStream.h>
#include <MW/Audio/WavStream.h>
#include <MW/IO/File.h>
#include <AL/al.h>
#include <AL/alext.h>

namespace mw {
inline namespace audio {

AudioClip::AudioClip(
    const path& filePath,
    AudioFileFormat format,
    bool streamFromDisk)
    : filePath(filePath)
    , format(format)
    , streamFromDisk(streamFromDisk)
{
    auto stream = OpenAudioStream();

    frameCount = stream->GetFrameCount();
    sampleRate = stream->GetSampleRate();
    channelCount = stream->GetChannels();
    bitRate = stream->GetBytesPerSample() * 8;
    length = (float)frameCount / sampleRate;

    if (channelCount != 1 && channelCount != 2)
        throw std::runtime_error("audio data must have 1 or 2 channels");

    SampleType sampleType = stream->GetSampleType();
    bool isMono = (channelCount == 1);

    switch (sampleType)
    {
    case SampleType::Int8:
        bufferFormat = isMono ? AL_FORMAT_MONO8 : AL_FORMAT_STEREO8;
        break;

    case SampleType::Int16:
        bufferFormat = isMono ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
        break;

    case SampleType::Float32:
        bufferFormat = isMono ? AL_FORMAT_MONO_FLOAT32 : AL_FORMAT_STEREO_FLOAT32;
        break;

    case SampleType::Float64:
        bufferFormat = isMono ? AL_FORMAT_MONO_DOUBLE_EXT : AL_FORMAT_STEREO_DOUBLE_EXT;
        break;

    default:
        throw std::runtime_error("unsupported audio format");
    }

    // if not streaming, create one shared buffer
    if (!streamFromDisk)
    {
        size_t dataSize = stream->GetLength();

        std::vector<std::byte> data;
        data.resize(dataSize);

        size_t totalBytesRead = 0;

        while (totalBytesRead != dataSize)
        {
            auto buff = std::span<std::byte>(
                data.data() + totalBytesRead,
                dataSize - totalBytesRead);

            int bytesRead = stream->Read(buff);
            if (bytesRead == 0)
                break;

            totalBytesRead += bytesRead;
        }

        if (totalBytesRead != dataSize)
            throw std::runtime_error("unexpected end of stream");

        alGenBuffers(1, &buffer);
        alBufferData(buffer, bufferFormat, data.data(), dataSize, sampleRate);
    }
}

AudioClip::~AudioClip()
{
    if (alIsBuffer(buffer))
        alDeleteBuffers(1, &buffer);

    buffer = 0;
}

sptr<AudioStream> AudioClip::OpenAudioStream() const
{
    auto file = File::Open(filePath, OpenMode::In | OpenMode::Binary);
    if (!file)
        throw std::runtime_error("could not open file");

    sptr<AudioStream> stream;

    if (format == AudioFileFormat::Mp3)
        stream = spnew<Mp3Stream>(file);
    else if (format == AudioFileFormat::Ogg)
        stream = spnew<OggStream>(file);
    else if (format == AudioFileFormat::Wav)
        stream = spnew<WavStream>(file);

    return stream;
}

} // audio
} // mw
