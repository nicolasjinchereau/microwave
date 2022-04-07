/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Audio/AudioStream.h>
#include <MW/System/Object.h>
#include <MW/System/path.h>

namespace mw {
inline namespace scene {
class AudioSource;
}

inline namespace audio {

enum class AudioFileFormat
{
    Mp3,
    Ogg,
    Wav
};

class AudioClip : public Object
{
    REGISTER_OBJECT(AudioClip);

    path filePath;
    AudioFileFormat format = {};
    bool streamFromDisk = {};

    uint32_t buffer = 0;
    int bufferFormat = 0;
    int frameCount = 0;
    int sampleRate = 0;
    int channelCount = 0;
    int bitRate = 0;
    float length = 0;
    
    friend class AudioSource;
public:
    AudioClip() {}
    AudioClip(const path& filePath, AudioFileFormat format, bool streamFromDisk);
    ~AudioClip();

    // length in frames
    int GetFrameCount() const { return frameCount; }
    
    // frames per sec
    int GetSampleRate() const { return sampleRate; }
    
    // samples per frame
    int GetChannels() const { return channelCount; }
    
    // bits per sample
    int GetBitrate() const { return bitRate; }
    
    // length in seconds
    float GetLength() const { return length; }

    AudioFileFormat GetFormat() const { return format; }

    int GetBufferFormat() const { return bufferFormat; }

    bool IsStreamedFromDisk() const { return streamFromDisk; }

    sptr<AudioStream> OpenAudioStream() const;

    // if 'IsStreamedFromDisk' is false, returns the shared audio buffer
    uint32_t GetBufferID() const { return buffer; }
};

inline void to_json(json& obj, const AudioFileFormat& fileFormat)
{
    static std::unordered_map<AudioFileFormat, std::string> formats{
        { AudioFileFormat::Mp3, "Mp3" },
        { AudioFileFormat::Ogg, "Ogg" },
        { AudioFileFormat::Wav, "Wav" }
    };
    obj = formats[fileFormat];
}

inline void from_json(const json& obj, AudioFileFormat& fileFormat)
{
    static std::unordered_map<std::string, AudioFileFormat> formats{
        { "Mp3", AudioFileFormat::Mp3 },
        { "Ogg", AudioFileFormat::Ogg },
        { "Wav", AudioFileFormat::Wav }
    };
    fileFormat = formats[obj.get<std::string>("Wav")];
}

} // audio
} // mw
