/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Image;
import Microwave.Graphics.GraphicsTypes;
import Microwave.IO.FileStream;
import Microwave.Math;
import Microwave.System.Json;
import Microwave.System.Path;
import Microwave.System.Pointers;
import <cstddef>;
import <cstdint>;
import <memory>;
import <span>;
import <string>;
import <unordered_map>;
import <vector>;

export namespace mw {
inline namespace gfx {

enum class ImageFileFormat
{
    TGA,
    PNG,
    JPG,
    EXR
};

struct ImageInfo
{
    PixelDataFormat format = PixelDataFormat::Unspecified;
    IVec2 size;
};

class Image
{
protected:
    IVec2 size;
    PixelDataFormat format = PixelDataFormat::Unspecified;
    uptr<std::byte[]> data;

public:
    Image();
    Image(Image&&) = default;
    Image(const path& p);
    Image(const path& p, ImageFileFormat fileFormat);
    Image(const sptr<FileStream>& stream);
    Image(const sptr<FileStream>& stream, ImageFileFormat fileFormat);
    Image(PixelDataFormat pixelFormat, const IVec2& size);
    Image(PixelDataFormat pixelFormat, const IVec2& size, std::span<std::byte> pixelData);
    Image(ImageFileFormat fileFormat, std::span<std::byte> fileData);
    ~Image();

    Image& operator=(Image&&) = default;

    static ImageInfo GetInfo(const path& p);
    static ImageInfo GetInfo(const path& p, ImageFileFormat fileFormat);
    static ImageInfo GetInfoTGA(const path& p);
    static ImageInfo GetInfoPNG(const path& p);
    static ImageInfo GetInfoJPG(const path& p);
    static ImageInfo GetInfoEXR(const path& p);

    void Load(PixelDataFormat pixelFormat, const IVec2& size);
    void Load(PixelDataFormat pixelFormat, const IVec2& size, std::span<std::byte> pixelData);

    void Load(const sptr<FileStream>& stream);
    void Load(const sptr<FileStream>& stream, ImageFileFormat fileFormat);
    void Load(ImageFileFormat fileFormat, std::span<std::byte> fileData);
    void LoadTGA(std::span<std::byte> fileData);
    void LoadPNG(std::span<std::byte> fileData);
    void LoadJPG(std::span<std::byte> fileData);
    void LoadEXR(std::span<std::byte> fileData);

    void SaveTGA(std::vector<std::byte>& fileData);
    void SavePNG(std::vector<std::byte>& fileData);
    void SaveJPG(std::vector<std::byte>& fileData);
    void SaveEXR(std::vector<std::byte>& fileData);

    void Save(ImageFileFormat format, std::vector<std::byte>& output);
    void Clear();

    void FlipHorizontally();
    void FlipVertically();

    IVec2 GetSize() const;
    PixelDataFormat GetFormat() const;

    std::span<std::byte> GetData();
    std::byte* GetPixel(uint32_t x, uint32_t y);

    // 'format' of 'Unspecified' uses original format
    Image Clone(PixelDataFormat format = PixelDataFormat::Unspecified);

    static void Blit(
        std::byte* src, IVec2 srcSize, IntRect srcRect, PixelDataFormat srcFormat,
        std::byte* dst, IVec2 dstSize, IVec2 dstPos, PixelDataFormat dstFormat,
        bool flipVertically = false);

    static ImageFileFormat GetFormatForFile(const path& filename);
};

void to_json(json& obj, const ImageFileFormat& fileFormat)
{
    static std::unordered_map<ImageFileFormat, std::string> formats{
        { ImageFileFormat::TGA, "TGA" },
        { ImageFileFormat::PNG, "PNG" },
        { ImageFileFormat::JPG, "JPG" },
        { ImageFileFormat::EXR, "EXR" }
    };
    obj = formats[fileFormat];
}

void from_json(const json& obj, ImageFileFormat& fileFormat)
{
    static std::unordered_map<std::string, ImageFileFormat> formats{
        { "TGA", ImageFileFormat::TGA },
        { "PNG", ImageFileFormat::PNG },
        { "JPG", ImageFileFormat::JPG },
        { "EXR", ImageFileFormat::EXR }
    };
    fileFormat = formats[obj.get<std::string>("PNG")];
}

} // gfx
} // mw

module: private;


