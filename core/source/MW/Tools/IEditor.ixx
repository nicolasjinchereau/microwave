/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Tools.IEditor;
import std;

export namespace mw {
inline namespace tools {

class IEditor
{
public:
    virtual ~IEditor() = default;

    virtual void SetViewportSize(int width, int height) = 0;
    virtual void Render() = 0;
    virtual void GetPixels(std::byte* pBuffer, std::size_t size) = 0;
};

} // tools
} // mw
