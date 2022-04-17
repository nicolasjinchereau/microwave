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

    virtual size_t Write(const void* pData, FbxUInt64 pSize) {
        assert(0); // read only
        return 0;
    }

    virtual size_t Read(void* pData, FbxUInt64 pSize) const
    {
        std::byte* pBuffer = (std::byte*)pData;
        auto buffer = std::span<std::byte>(pBuffer, (size_t)pSize);
        return (size_t)stream->Read(buffer);
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

    virtual FbxInt64 GetPosition() const {
        return (FbxInt64)stream->GetPosition();
    }

    virtual void SetPosition(FbxInt64 pPosition) {
        stream->SetPosition((size_t)pPosition);
    }

    virtual int GetError() const {
        return 0;
    }

    virtual void ClearError() {

    }
};

}
}
