/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/IO/Stream.h>
#include <vorbis/vorbisfile.h>
#include <cstdint>

namespace mw {
inline namespace audio {

class OggDecoder
{
    OggVorbis_File file{};
    vorbis_info* info{};
public:
    OggDecoder(Stream* stream)
    {
        ov_callbacks callbacks;
        callbacks.read_func = OggRead;
        callbacks.seek_func = OggSeek;
        callbacks.close_func = OggClose;
        callbacks.tell_func = OggTell;

        int ret = ov_open_callbacks((void*)stream, &file, NULL, -1, callbacks);
        if (ret != 0)
            throw std::runtime_error("couldn't open ogg file");

        info = ov_info(&file, -1);
    }

    ~OggDecoder() {
        ov_clear(&file);
    }

    int64_t sampleRate() const {
        return info->rate;
    }

    int channels() const {
        return info->channels;
    }

    int64_t pcmTotal() const {
        return ov_pcm_total((OggVorbis_File*)&file, -1);
    };

    int pcmSeek(int64_t pos) const {
        return ov_pcm_seek((OggVorbis_File*)&file, pos);
    }

    int64_t pcmTell() const {
        return ov_pcm_tell((OggVorbis_File*)&file);
    }

    int64_t readFloat(float*** sampleChannels, int frameCount, int* bitstream = nullptr)
    {
        int bs{};
        int* pBs = bitstream ? bitstream : &bs;
        return ov_read_float((OggVorbis_File*)&file, sampleChannels, frameCount, pBs);
    }

    static size_t OggRead(void* ptr, size_t size, size_t count, void* datasource)
    {
        size_t len = size * count;
        Stream* stream = (Stream*)datasource;
        return stream->Read(std::span<std::byte>((std::byte*)ptr, len));
    }

    static int OggSeek(void* datasource, ogg_int64_t offset, int origin)
    {
        static std::array<SeekOrigin, 3> origins{
            SeekOrigin::Begin,   // SEEK_SET
            SeekOrigin::Current, // SEEK_CUR
            SeekOrigin::End      // SEEK_END
        };

        Stream* stream = (Stream*)datasource;
        return stream->Seek(offset, origins[origin]);
    }

    static int OggClose(void* datasource)
    {
        Stream* stream = (Stream*)datasource;
        stream->Close();
        return 0;
    }

    static long OggTell(void* datasource)
    {
        Stream* stream = (Stream*)datasource;
        return (long)stream->GetPosition();
    }
};

} // audio
} // mw
