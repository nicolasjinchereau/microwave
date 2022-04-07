/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/IO/Stream.h>
#include <MW/System/Pointers.h>
#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <span>
#include <fbxsdk.h>

namespace mw {
inline namespace data {

class FbxMemoryStream : public fbxsdk::FbxStream
{
    sptr<Stream> stream;
    mutable std::span<std::byte>::iterator pos;
    int readerId;
    int writerId;
public:
    FbxMemoryStream(
        fbxsdk::FbxManager* pSdkManager,
        const sptr<Stream>& stream)
        : stream(stream)
    {
        readerId = pSdkManager->GetIOPluginRegistry()->FindReaderIDByDescription("FBX (*.fbx)");
        writerId = -1;
    }

    ~FbxMemoryStream() {
        Close();
    }

    virtual EState GetState() {
        return stream->CanRead() ? fbxsdk::FbxStream::eOpen : fbxsdk::FbxStream::eClosed;
    }

    virtual bool Open(void* /*pStreamData*/) {
        stream->Seek(0, SeekOrigin::Begin);
        return stream->CanRead();
    }

    virtual bool Close() {
        stream->Seek(0, SeekOrigin::Begin);
        return true;
    }

    virtual bool Flush() {
        return true;
    }

    virtual int Write(const void* pData, int pSize) {
        assert(0); // read only
        return 0;
    }

    virtual int Read(void* pData, int pSize) const
    {
        std::byte* pBuffer = (std::byte*)pData;
        return stream->Read(std::span<std::byte>(pBuffer, pSize));
    }

    virtual int GetReaderID() const {
        return readerId;
    }

    virtual int GetWriterID() const {
        return writerId;
    }

    void Seek(const fbxsdk::FbxInt64& pOffset, const fbxsdk::FbxFile::ESeekPos& pSeekPos)
    {
        switch (pSeekPos)
        {
        case fbxsdk::FbxFile::eBegin:
            stream->Seek(pOffset, SeekOrigin::Begin);
            break;

        case fbxsdk::FbxFile::eCurrent:
            stream->Seek(pOffset, SeekOrigin::Current);
            break;

        case fbxsdk::FbxFile::eEnd:
            stream->Seek(pOffset, SeekOrigin::End);
            break;
        }
    }

    virtual long GetPosition() const {
        return (long)stream->GetPosition();
    }

    virtual void SetPosition(long pPosition) {
        stream->SetPosition(pPosition);
    }

    virtual int GetError() const {
        return 0;
    }

    virtual void ClearError() {

    }
};

}
}
