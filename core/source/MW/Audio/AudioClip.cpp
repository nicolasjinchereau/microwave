/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Audio.AudioClip;
import Microwave.Audio.AudioStream;
import Microwave.Audio.Mp3Stream;
import Microwave.Audio.OggStream;
import Microwave.Audio.WavStream;
import Microwave.IO.File;
import Microwave.SceneGraph.Components.AudioSource;
import Microwave.System.Exception;
import Microwave.System.Path;
import Microwave.System.Pointers;
import <stdexcept>;
import <cstddef>;
import <cstdint>;
import <vector>;
import <MW/Audio/Internal/OpenAL.h>;

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
        throw Exception("audio data must have 1 or 2 channels");

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
        throw Exception("unsupported audio format");
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
            throw Exception("unexpected end of stream");

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

gptr<AudioStream> AudioClip::OpenAudioStream() const
{
    auto file = File::Open(filePath, OpenMode::In | OpenMode::Binary);
    if (!file)
        throw Exception("could not open file");

    gptr<AudioStream> stream;

    if (format == AudioFileFormat::Mp3)
        stream = gpnew<Mp3Stream>(file);
    else if (format == AudioFileFormat::Ogg)
        stream = gpnew<OggStream>(file);
    else if (format == AudioFileFormat::Wav)
        stream = gpnew<WavStream>(file);

    return stream;
}

} // audio
} // mw
