/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Audio.Internal.Mp3Decoder;
import Microwave.IO.Stream;
import Microwave.System.Exception;
import <MW/System/Debug.h>;
import <dr-mp3.h>;
import std;

export namespace mw {
inline namespace audio {

class Mp3Decoder
{
    drmp3 mp3{};
public:

    Mp3Decoder(Stream* stream)
    {
        auto ret = drmp3_init(&mp3, Mp3Read, Mp3Seek, stream, nullptr);
        if (!ret)
            throw Exception("failed to open stream");
    }

    ~Mp3Decoder() {
        drmp3_uninit(&mp3);
    }

    std::uint32_t sampleRate() const {
        return mp3.sampleRate;
    }

    std::uint32_t channels() const {
        return mp3.channels;
    }

    std::uint64_t pcmFrameCount() const {
        return drmp3_get_pcm_frame_count((drmp3*)&mp3);
    }

    bool SeekToPCMFrame(std::uint64_t frameIndex) {
        return drmp3_seek_to_pcm_frame((drmp3*)&mp3, frameIndex) == DRMP3_TRUE;
    }

    std::uint64_t ReadPCMFrames(std::uint64_t framesToRead, std::int16_t* pBufferOut) {
        return drmp3_read_pcm_frames_s16((drmp3*)&mp3, framesToRead, pBufferOut);
    }

    static std::size_t Mp3Read(void* pUserData, void* pBufferOut, std::size_t bytesToRead)
    {
        Stream* stream = (Stream*)pUserData;
        return stream->Read(std::span<std::byte>((std::byte*)pBufferOut, bytesToRead));
    }

    static drmp3_bool32 Mp3Seek(void* pUserData, int offset, drmp3_seek_origin origin)
    {
        SeekOrigin orig;

        if (origin == drmp3_seek_origin_start)
            orig = SeekOrigin::Begin;
        else if (origin == drmp3_seek_origin_current)
            orig = SeekOrigin::Current;
        else
            Assert(0);

        Stream* stream = (Stream*)pUserData;
        auto newPos = stream->Seek(offset, orig);
        return DRMP3_TRUE;
    }
};

} // audio
} // mw
