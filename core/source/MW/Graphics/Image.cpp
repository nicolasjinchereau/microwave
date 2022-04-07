/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/Graphics/Image.h>
#include <MW/IO/File.h>
#include <MW/Utilities/Util.h>
#include <png.h>
#include <jpeglib.h>
#include <tinyexr.h>

namespace mw {
inline namespace gfx {

enum class TargaType : uint8_t
{
    None = 0,
    ColorMapped = 1,    // 0b1
    TrueColor = 2,      // 0b2
    Grayscale = 3,      // 0b3
    ColorMappedRLE = 9, // 0b1001
    TrueColorRLE = 10,  // 0b1010
    GrayscaleRLE = 11   // 0b1011
};

struct TargaHeader
{
    uint8_t idLength;        // length of image ID section
    uint8_t colorMapType;    // 1<<0 set if color map present
    TargaType imageType;
    uint16_t colorMapStartIndex;
    uint16_t colorMapLength;
    uint8_t colorMapBitDepth;
    uint16_t imageOriginX;
    uint16_t imageOriginY;
    uint16_t imageWidth;
    uint16_t imageHeight;
    uint8_t imageBitDepth;
    uint8_t imageDescriptor; // alpha depth(0-3), unused(4), upper-left-origin(5), interleaving(6-7)

    static TargaHeader Unpack(uint8_t (&bytes)[18])
    {
        return {
            bytes[0],
            bytes[1],
            (TargaType)bytes[2],
            (uint16_t)(bytes[4] << 8 | bytes[3]),
            (uint16_t)(bytes[6] << 8 | bytes[5]),
            bytes[7],
            (uint16_t)(bytes[9]  << 8 | bytes[8]),
            (uint16_t)(bytes[11] << 8 | bytes[10]),
            (uint16_t)(bytes[13] << 8 | bytes[12]),
            (uint16_t)(bytes[15] << 8 | bytes[14]),
            bytes[16],
            bytes[17],
        };
    }
};

Image::Image() {

}

Image::Image(const path& p) {
    auto stream = File::Open(p, OpenMode::In | OpenMode::Binary);
    Load(stream);
}

Image::Image(const path& p, ImageFileFormat fileFormat) {
    auto stream = File::Open(p, OpenMode::In | OpenMode::Binary);
    Load(stream, fileFormat);
}

Image::Image(const sptr<FileStream>& stream) {
    Load(stream);
}

Image::Image(const sptr<FileStream>& stream, ImageFileFormat fileFormat) {
    Load(stream, fileFormat);
}

Image::Image(PixelDataFormat pixelFormat, const IVec2& size) {
    Load(pixelFormat, size);
}

Image::Image(PixelDataFormat pixelFormat, const IVec2& size, std::span<std::byte> pixelData) {
    Load(pixelFormat, size, pixelData);
}

Image::Image(ImageFileFormat fileFormat, std::span<std::byte> fileData) {
    Load(fileFormat, fileData);
}

Image::~Image()
{

}

void Image::Load(PixelDataFormat pixelFormat, const IVec2& size)
{
    this->format = pixelFormat;
    this->size = size;
    auto count = size.x * size.y * GetBytesPerPixel(format);
    data = std::make_unique<std::byte[]>(count);
    std::fill(data.get(), data.get() + count, std::byte(0));
}

void Image::Load(PixelDataFormat pixelFormat, const IVec2& size, std::span<std::byte> pixelData)
{
    this->format = pixelFormat;
    this->size = size;
    auto count = size.x * size.y * GetBytesPerPixel(format);
    data = std::make_unique<std::byte[]>(count);
    std::copy(pixelData.begin(), pixelData.end(), data.get());
}

void Image::Load(const sptr<FileStream>& stream)
{
    auto path = stream->GetPath();
    auto ext = ToLower(path.extension());
    
    if (ext == ".tga")
    {
        auto fileData = File::ReadAllBytes(path);
        LoadTGA(fileData);
    }
    else if (ext == ".png")
    {
        auto fileData = File::ReadAllBytes(path);
        LoadPNG(fileData);
    }
    else if (ext == ".jpg")
    {
        auto fileData = File::ReadAllBytes(path);
        LoadJPG(fileData);
    }
    else if (ext == ".exr")
    {
        auto fileData = File::ReadAllBytes(path);
        LoadEXR(fileData);
    }
    else
    {
        throw std::runtime_error("unsupported file type");
    }
}

void Image::Load(const sptr<FileStream>& stream, ImageFileFormat fileFormat)
{
    if (fileFormat == ImageFileFormat::TGA)
    {
        auto fileData = File::ReadAllBytes(stream->GetPath());
        LoadTGA(fileData);
    }
    else if (fileFormat == ImageFileFormat::PNG)
    {
        auto fileData = File::ReadAllBytes(stream->GetPath());
        LoadPNG(fileData);
    }
    else if (fileFormat == ImageFileFormat::JPG)
    {
        auto fileData = File::ReadAllBytes(stream->GetPath());
        LoadJPG(fileData);
    }
    else if (fileFormat == ImageFileFormat::EXR)
    {
        auto fileData = File::ReadAllBytes(stream->GetPath());
        LoadEXR(fileData);
    }
    else
    {
        throw std::runtime_error("unsupported file type");
    }
}

void Image::Load(ImageFileFormat fileFormat, std::span<std::byte> fileData)
{
    switch (fileFormat)
    {
    case ImageFileFormat::TGA:
        LoadTGA(fileData);
        break;

    case ImageFileFormat::PNG:
        LoadPNG(fileData);
        break;

    case ImageFileFormat::JPG:
        LoadJPG(fileData);
        break;

    case ImageFileFormat::EXR:
        LoadEXR(fileData);
        break;
    }
}

void ExpectBytes(const std::span<std::byte>& byte, ptrdiff_t count)
{
    if (byte.size() < (size_t)count)
        throw std::runtime_error("unexpected end of data");
}

void Image::LoadTGA(std::span<std::byte> fileData)
{
    auto sz = sizeof(TargaHeader);

    ExpectBytes(fileData, 18);
    uint8_t hdrBytes[18];
    memcpy(hdrBytes, fileData.data(), sizeof(hdrBytes));
    fileData = fileData.subspan(sizeof(hdrBytes));
    TargaHeader hdr = TargaHeader::Unpack(hdrBytes);

    bool hasColorMap = (hdr.colorMapType & 1) != 0;
    TargaType type = hdr.imageType;
    int bytesPerPixel = hdr.imageBitDepth / 8;

    if(type != TargaType::TrueColor
    && type != TargaType::TrueColorRLE)
        throw std::runtime_error("Invalid file format. Only true-color TGA files are supported.");

    if(hdr.imageBitDepth != 24
    && hdr.imageBitDepth != 32)
        throw std::runtime_error("Invalid file format. Only 24 and 32 bit files are supported.");

    if (hdr.idLength > 0) {
        auto sz = hdr.idLength;
        ExpectBytes(fileData, sz);
        fileData = fileData.subspan(sz);
    }

    if (hdr.colorMapLength > 0) {
        auto sz = hdr.colorMapLength * (hdr.colorMapBitDepth / 8);
        ExpectBytes(fileData, sz);
        fileData = fileData.subspan(sz);
    }

    int totalPixels = hdr.imageWidth * hdr.imageHeight;
    std::unique_ptr<std::byte[]> tmpData = std::make_unique<std::byte[]>(totalPixels * bytesPerPixel);

    if(type == TargaType::TrueColor)
    {
        std::byte* pOutput = tmpData.get();

        auto sz = totalPixels * bytesPerPixel;
        ExpectBytes(fileData, sz);
        auto pData = fileData.data();
        fileData = fileData.subspan(sz);

        for(int i = 0; i < totalPixels; ++i)
        {
            *pOutput++ = pData[2];
            *pOutput++ = pData[1];
            *pOutput++ = pData[0];

            if (bytesPerPixel == 4)
                *pOutput++ = pData[3];

            pData += bytesPerPixel;
        }
    }
    else if(type == TargaType::TrueColorRLE)
    {
        int count = 0;
        std::byte* pOutput = tmpData.get();
        
        while(count < totalPixels)
        {
            ExpectBytes(fileData, 1);
            uint8_t chunkHdr = (uint8_t)fileData[0];
            fileData = fileData.subspan(1);

            bool isRLE = (chunkHdr & 0b10000000) != 0;
            int chunkLength = (chunkHdr & 0b01111111) + 1;
            count += chunkLength;

            if(isRLE)
            {
                ExpectBytes(fileData, bytesPerPixel);
                auto pData = fileData.data();
                fileData = fileData.subspan(bytesPerPixel);

                std::byte rgba[4];

                rgba[0] = pData[2];
                rgba[1] = pData[1];
                rgba[2] = pData[0];

                if (bytesPerPixel == 4)
                    rgba[3] = pData[3];

                pData += bytesPerPixel;

                for (int i = 0; i < chunkLength; ++i)
                {
                    *pOutput++ = rgba[0];
                    *pOutput++ = rgba[1];
                    *pOutput++ = rgba[2];

                    if (bytesPerPixel == 4)
                        *pOutput++ = rgba[3];
                }
            }
            else
            {
                auto sz = chunkLength * bytesPerPixel;
                ExpectBytes(fileData, sz);
                auto pData = fileData.data();
                fileData = fileData.subspan(sz);

                for(int i = 0; i < chunkLength; ++i)
                {
                    *pOutput++ = pData[2];
                    *pOutput++ = pData[1];
                    *pOutput++ = pData[0];

                    if (bytesPerPixel == 4)
                        *pOutput++ = pData[3];

                    pData += bytesPerPixel;
                }
            }
        }
    }

    bool vflipped = (hdr.imageDescriptor & 0b10000) == 0;
    if(vflipped)
    {
        int h = hdr.imageHeight;
        int w = hdr.imageWidth;

        for(size_t y1 = 0, ye = h / 2;
            y1 < ye; ++y1)
        {
            size_t y2 = h - y1 - 1;

            std::byte* r1_beg = tmpData.get() + y1 * w * bytesPerPixel;
            std::byte* r1_end = r1_beg + w * bytesPerPixel;

            std::byte* r2_beg = tmpData.get() + y2 * w * bytesPerPixel;
            std::byte* r2_end = r2_beg + w * bytesPerPixel;

            while(r1_beg != r1_end)
            {
                std::byte p = *r1_beg;
                *r1_beg = *r2_beg;
                *r2_beg = p;

                ++r1_beg;
                ++r2_beg;
            }
        }
    }

    // ignore extension section
    // ignore footer section

    this->size = IVec2(hdr.imageWidth, hdr.imageHeight);
    this->format = bytesPerPixel == 3 ? PixelDataFormat::RGB24 : PixelDataFormat::RGBA32;
    this->data = std::move(tmpData);
}

void Image::LoadPNG(std::span<std::byte> fileData)
{
    png_struct* pPngStruct = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!pPngStruct) {
        throw std::runtime_error("failed to initialize libpng.");
    }

    png_info* pPngInfo = png_create_info_struct(pPngStruct);
    if (!pPngInfo) {
        png_destroy_read_struct(&pPngStruct, NULL, NULL);
        throw std::runtime_error("failed to initialize libpng.");
    }

    const int PngSigSize = 8;

    ExpectBytes(fileData, PngSigSize);

    if (png_sig_cmp((png_bytep)fileData.data(), 0, PngSigSize) != 0) {
        throw std::runtime_error("error: invalid png file.");
    }

    std::unique_ptr<std::byte[]> tmpData;
    IVec2 tmpSize;

    if (setjmp(png_jmpbuf(pPngStruct)) == 0)
    {
        png_set_read_fn(pPngStruct, &fileData, [](png_structp png_ptr, png_bytep out_bytes, png_size_t length)
        {
            std::span<std::byte>& fileData = *(std::span<std::byte>*)png_get_io_ptr(png_ptr);
            
            if (fileData.size() < (ptrdiff_t)length) {
                png_error(png_ptr, "unexpected end of data");
                return;
            }

            memcpy(out_bytes, fileData.data(), length);
            fileData = fileData.subspan(length);
        });

        png_set_sig_bytes(pPngStruct, PngSigSize);
        fileData = fileData.subspan(PngSigSize);

        // get image dimensions
        png_read_info(pPngStruct, pPngInfo);
        tmpSize.x = png_get_image_width(pPngStruct, pPngInfo);
        tmpSize.y = png_get_image_height(pPngStruct, pPngInfo);

        // bits per channel
        uint32_t bitDepth = png_get_bit_depth(pPngStruct, pPngInfo);

        // (RGB, RGBA, Luminance, luminance alpha... palette... etc)
        uint32_t colorType = png_get_color_type(pPngStruct, pPngInfo);

        // convert to 8 bits per channel
        if (bitDepth == 16)
            png_set_strip_16(pPngStruct);

        // expand grayscale
        if (colorType == PNG_COLOR_TYPE_GRAY && bitDepth < 8)
            png_set_expand_gray_1_2_4_to_8(pPngStruct);

        // expand to RGBA
        if (png_get_valid(pPngStruct, pPngInfo, PNG_INFO_tRNS))
            png_set_tRNS_to_alpha(pPngStruct);

        // fill last byte of 24bit images
        png_set_filler(pPngStruct, 0xFF, PNG_FILLER_AFTER);

        // required for palette alterations
        png_read_update_info(pPngStruct, pPngInfo);

        // allocate storage for pixels
        tmpData = std::make_unique<std::byte[]>(tmpSize.x * tmpSize.y * 4);

        // make pointers to pixel rows
        std::unique_ptr<std::byte*[]> rowPtrs = std::make_unique<std::byte*[]>(tmpSize.y);

        for (int y = 0; y < tmpSize.y; ++y)
            rowPtrs[y] = (std::byte*)(tmpData.get() + y * tmpSize.x * 4);

        png_read_image(pPngStruct, (png_bytepp)rowPtrs.get());

        png_destroy_read_struct(&pPngStruct, &pPngInfo, NULL);
    }
    else
    {
        std::string error = (const char*)png_get_error_ptr(pPngStruct);
        png_destroy_read_struct(&pPngStruct, &pPngInfo, NULL);
        throw std::runtime_error(!error.empty() ? error : "could not read png file");
    }

    size = tmpSize;
    format = PixelDataFormat::RGBA32;
    data = std::move(tmpData);
}

void Image::LoadJPG(std::span<std::byte> fileData)
{
    jpeg_error_mgr jerr;
    jpeg_decompress_struct cinfo;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    jpeg_mem_src(&cinfo, (unsigned char*)fileData.data(), fileData.size());

    if(jpeg_read_header(&cinfo, TRUE) != 1)
    {
        jpeg_destroy_decompress(&cinfo);
        throw std::runtime_error("couldn't read jpg file");
    }

    jpeg_start_decompress(&cinfo);

    IVec2 tmpSize = { (int)cinfo.output_width, (int)cinfo.output_height };
    int channels = cinfo.output_components;
    int stride = cinfo.output_width * channels;

    if(channels != 3)
    {
        jpeg_destroy_decompress(&cinfo);
        throw std::runtime_error("unsupported jpeg format");
    }

    auto tmpData = std::make_unique<std::byte[]>(tmpSize.x * tmpSize.y * channels);

    for(std::byte* pRow = tmpData.get();
        (int)cinfo.output_scanline < tmpSize.y;
        pRow += stride)
    {
        jpeg_read_scanlines(&cinfo, (unsigned char**)&pRow, 1);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    size = tmpSize;
    format = PixelDataFormat::RGB24;
    data = std::move(tmpData);
}

void Image::LoadEXR(std::span<std::byte> fileData)
{
    float* outRGBA = nullptr;
    const char* err = nullptr;
    IVec2 tmpSize;

    int ret = LoadEXRFromMemory(
        &outRGBA, &tmpSize.x, &tmpSize.y,
        (const uint8_t*)fileData.data(), fileData.size(),
        &err);

    if (ret < 0)
    {
        std::string message;

        if (err) {
            message = err;
            FreeEXRErrorMessage(err);
        }
        else {
            message = "failed to load EXR file";
        }

        throw std::runtime_error(message);
    }

    auto sz = tmpSize.x * tmpSize.y * 4 * sizeof(float);
    data = std::make_unique<std::byte[]>(sz);
    memcpy(data.get(), outRGBA, sz);
    free(outRGBA);

    size = tmpSize;
    format = PixelDataFormat::RGBAFloat;
}

void Image::Save(ImageFileFormat format, std::vector<std::byte>& output)
{
    switch (format)
    {
    case ImageFileFormat::TGA:
        throw std::runtime_error("not supported");
        break;

    case ImageFileFormat::PNG:
        SavePNG(output);
        break;

    case ImageFileFormat::JPG:
        SaveJPG(output);
        break;

    case ImageFileFormat::EXR:
        throw std::runtime_error("not supported");
        break;
    }
}

void Image::SaveTGA(std::vector<std::byte>& fileData)
{
    fileData.clear();

}

void Image::SavePNG(std::vector<std::byte>& fileData)
{
    fileData.clear();

    png_struct *pPngStruct = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!pPngStruct)
        throw std::runtime_error("failed to initialize libpng.");

    png_info* pPngInfo = png_create_info_struct(pPngStruct);
    if (!pPngInfo) {
        png_destroy_write_struct(&pPngStruct, NULL);
        throw std::runtime_error("failed to initialize libpng.");
    }

    if(setjmp(png_jmpbuf(pPngStruct)) == 0)
    {
        png_set_write_fn(pPngStruct, &fileData, [](png_structp png_ptr, png_bytep data, png_size_t length)
        {
            auto bytes = (std::byte*)data;
            std::vector<std::byte>& fileData = *(std::vector<std::byte>*)png_get_io_ptr(png_ptr);
            fileData.insert(fileData.end(), bytes, bytes + length);
        }, nullptr);

        png_set_IHDR(pPngStruct,
            pPngInfo,
            size.x,
            size.y,
            8,
            PNG_COLOR_TYPE_RGBA,
            PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_BASE,
            PNG_FILTER_TYPE_BASE);

        png_write_info(pPngStruct, pPngInfo);

        auto bytesPerPixel = GetBytesPerPixel(format);

        std::unique_ptr<uint8_t*[]> rowPtrs(new uint8_t*[size.y]);

        for(int y = 0; y < size.y; ++y)
            rowPtrs[y] = (uint8_t*)(data.get() + y * size.x * bytesPerPixel);

        png_write_image(pPngStruct, rowPtrs.get());
        png_write_end(pPngStruct, pPngInfo);
        png_destroy_write_struct(&pPngStruct, &pPngInfo);
    }
    else
    {
        std::string error = (const char*)png_get_error_ptr(pPngStruct);
        png_destroy_write_struct(&pPngStruct, &pPngInfo);
        throw std::runtime_error(!error.empty() ? error : "could not save png file");
    }
}

void Image::SaveJPG(std::vector<std::byte>& fileData)
{
    throw std::runtime_error("not fully implemented: won't handle float pixels");

    fileData.clear();

    unsigned char* buffer = nullptr;
    size_t bufferSize = 0;
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_mem_dest(&cinfo, &buffer, &bufferSize);

    cinfo.image_width = size.x;
    cinfo.image_height = size.y;
    cinfo.input_components = 4; // GetBytesPerPixel(format);
    cinfo.in_color_space = JCS_RGB;
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 75, TRUE);
    jpeg_start_compress(&cinfo, TRUE);

    JSAMPROW row_pointer[1];
    auto rowStride = cinfo.image_width * cinfo.input_components;

    while (cinfo.next_scanline < cinfo.image_height)
    {
        row_pointer[0] = (JSAMPLE*)&data[cinfo.next_scanline * rowStride];
        auto linesWritten = jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
    
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);

    fileData.assign((std::byte*)buffer, (std::byte*)buffer + bufferSize);
    free(buffer);
}

void Image::SaveEXR(std::vector<std::byte>& fileData)
{
    //fileData.clear();

    //EXRHeader header;
    //InitEXRHeader(&header);

    //EXRImage image;
    //InitEXRImage(&image);

    //image.num_channels = 4;

    //std::vector<float> images[3];
    //images[0].resize(width * height);
    //images[1].resize(width * height);
    //images[2].resize(width * height);

    //// Split RGBRGBRGB... into R, G and B layer
    //for (int i = 0; i < width * height; i++) {
    //    images[0][i] = rgb[3 * i + 0];
    //    images[1][i] = rgb[3 * i + 1];
    //    images[2][i] = rgb[3 * i + 2];
    //}

    //float* image_ptr[3];
    //image_ptr[0] = &(images[2].at(0)); // B
    //image_ptr[1] = &(images[1].at(0)); // G
    //image_ptr[2] = &(images[0].at(0)); // R

    //image.images = (unsigned char**)image_ptr;
    //image.width = width;
    //image.height = height;

    //header.num_channels = 3;
    //header.channels = (EXRChannelInfo*)malloc(sizeof(EXRChannelInfo) * header.num_channels);
    //// Must be (A)BGR order, since most of EXR viewers expect this channel order.
    //strncpy(header.channels[0].name, "B", 255); header.channels[0].name[strlen("B")] = '\0';
    //strncpy(header.channels[1].name, "G", 255); header.channels[1].name[strlen("G")] = '\0';
    //strncpy(header.channels[2].name, "R", 255); header.channels[2].name[strlen("R")] = '\0';

    //header.pixel_types = (int*)malloc(sizeof(int) * header.num_channels);
    //header.requested_pixel_types = (int*)malloc(sizeof(int) * header.num_channels);
    //for (int i = 0; i < header.num_channels; i++) {
    //    header.pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT; // pixel type of input image
    //    header.requested_pixel_types[i] = TINYEXR_PIXELTYPE_HALF; // pixel type of output image to be stored in .EXR
    //}

    //const char* err = NULL; // or nullptr in C++11 or later.
    //int ret = SaveEXRImageToFile(&image, &header, outfilename, &err);
    //if (ret != TINYEXR_SUCCESS) {
    //    fprintf(stderr, "Save EXR err: %s\n", err);
    //    FreeEXRErrorMessage(err); // free's buffer for an error message
    //    return ret;
    //}
    //printf("Saved exr file. [ %s ] \n", outfilename);

    //free(rgb);

    //free(header.channels);
    //free(header.pixel_types);
    //free(header.requested_pixel_types);
}


void Image::Clear()
{
    if (data) {
        auto count = size.x * size.y * GetBytesPerPixel(format);
        std::fill(data.get(), data.get() + count, std::byte(0));
    }
}

void Image::FlipHorizontally()
{

}

void Image::FlipVertically()
{

}

PixelDataFormat Image::GetFormat() const {
    return format;
}

IVec2 Image::GetSize() const {
    return size;
}

std::span<std::byte> Image::GetData() {
    return std::span<std::byte>(data.get(), size.x * size.y * GetBytesPerPixel(format));
}

std::byte* Image::GetPixel(uint32_t x, uint32_t y) {
    return data.get() + (y * size.x + x) * GetBytesPerPixel(format);
}

std::byte ConvertToGrayscale(std::byte r, std::byte g, std::byte b)
{
    uint32_t rfrac = (uint32_t)r * 2990000;
    uint32_t gfrac = (uint32_t)g * 5870000;
    uint32_t bfrac = (uint32_t)b * 1140000;
    uint32_t lum = ((rfrac + gfrac + bfrac) + 5000000) / 10000000;
    return (std::byte)std::min(std::max(lum, 0U), 255U);
}

void ConvertPixel_Alpha8ToAlpha8(std::byte*& src, std::byte*& dst) {
    *dst++ = *src++;
}

void ConvertPixel_Alpha8ToRGB24(std::byte*& src, std::byte*& dst) {
    std::byte alpha = *src++;
    *dst++ = alpha;
    *dst++ = alpha;
    *dst++ = alpha;
}

void ConvertPixel_Alpha8ToRGBA32(std::byte*& src, std::byte*& dst) {
    std::byte alpha = *src++;
    *dst++ = alpha;
    *dst++ = alpha;
    *dst++ = alpha;
    *dst++ = (std::byte)255;
}

void ConvertPixel_Alpha8ToRGBAFloat(std::byte*& src, std::byte*& dst)
{
    float alpha = math::Clamp01((float)*src++ / 255.0f);
    
    std::byte* pAlphaBytes = (std::byte*)&alpha;
    for (int i = 0; i != 4; ++i)
    {
        *dst++ = pAlphaBytes[0];
        *dst++ = pAlphaBytes[1];
        *dst++ = pAlphaBytes[2];
        *dst++ = pAlphaBytes[3];
    }
}

void ConvertPixel_RGB24ToAlpha8(std::byte*& src, std::byte*& dst)
{
    std::byte r = *src++;
    std::byte g = *src++;
    std::byte b = *src++;
    *dst++ = ConvertToGrayscale(r, g, b);
}

void ConvertPixel_RGB24ToRGB24(std::byte*& src, std::byte*& dst)
{
    *dst++ = *src++;
    *dst++ = *src++;
    *dst++ = *src++;
}

void ConvertPixel_RGB24ToRGBA32(std::byte*& src, std::byte*& dst)
{
    *dst++ = *src++;
    *dst++ = *src++;
    *dst++ = *src++;
    *dst++ = (std::byte)255;
}

void ConvertPixel_RGB24ToRGBAFloat(std::byte*& src, std::byte*& dst)
{
    float rgba[4];
    rgba[0] = math::Clamp01((float)*src++ / 255.0f);
    rgba[1] = math::Clamp01((float)*src++ / 255.0f);
    rgba[2] = math::Clamp01((float)*src++ / 255.0f);
    rgba[3] = 1.0f;

    for (int i = 0; i != 4; ++i)
    {
        std::byte* pChan = (std::byte*)&rgba[i];
        *dst++ = pChan[0];
        *dst++ = pChan[1];
        *dst++ = pChan[2];
        *dst++ = pChan[3];
    }
}

void ConvertPixel_RGBA32ToAlpha8(std::byte*& src, std::byte*& dst)
{
    std::byte r = *src++;
    std::byte g = *src++;
    std::byte b = *src++;
    src++;
    *dst++ = ConvertToGrayscale(r, g, b);
}

void ConvertPixel_RGBA32ToRGB24(std::byte*& src, std::byte*& dst)
{
    *dst++ = *src++;
    *dst++ = *src++;
    *dst++ = *src++;
    src++;
}

void ConvertPixel_RGBA32ToRGBA32(std::byte*& src, std::byte*& dst)
{
    *dst++ = *src++;
    *dst++ = *src++;
    *dst++ = *src++;
    *dst++ = *src++;
}

void ConvertPixel_RGBA32ToRGBAFloat(std::byte*& src, std::byte*& dst)
{
    float rgba[4];
    rgba[0] = math::Clamp01((float)*src++ / 255.0f);
    rgba[1] = math::Clamp01((float)*src++ / 255.0f);
    rgba[2] = math::Clamp01((float)*src++ / 255.0f);
    rgba[3] = math::Clamp01((float)*src++ / 255.0f);

    for (int i = 0; i != 4; ++i)
    {
        std::byte* pChan = (std::byte*)&rgba[i];
        *dst++ = pChan[0];
        *dst++ = pChan[1];
        *dst++ = pChan[2];
        *dst++ = pChan[3];
    }
}

void ConvertPixel_RGBAFloatToAlpha8(std::byte*& src, std::byte*& dst)
{
    float rgba[4];
    for (int i = 0; i != 4; ++i) {
        std::byte* pVal = (std::byte*)&rgba[i];
        *pVal++ = *src++;
        *pVal++ = *src++;
        *pVal++ = *src++;
        *pVal++ = *src++;
    }

    std::byte r = (std::byte)math::Clamp(math::RoundToInt(rgba[0] * 255.0f), 0, 255);
    std::byte g = (std::byte)math::Clamp(math::RoundToInt(rgba[1] * 255.0f), 0, 255);
    std::byte b = (std::byte)math::Clamp(math::RoundToInt(rgba[2] * 255.0f), 0, 255);
    *dst++ = ConvertToGrayscale(r, g, b);
}

void ConvertPixel_RGBAFloatToRGB24(std::byte*& src, std::byte*& dst)
{
    float rgba[4];
    for (int i = 0; i != 4; ++i) {
        std::byte* pVal = (std::byte*)&rgba[i];
        *pVal++ = *src++;
        *pVal++ = *src++;
        *pVal++ = *src++;
        *pVal++ = *src++;
    }

    *dst++ = (std::byte)math::Clamp(math::RoundToInt(rgba[0] * 255.0f), 0, 255);
    *dst++ = (std::byte)math::Clamp(math::RoundToInt(rgba[1] * 255.0f), 0, 255);
    *dst++ = (std::byte)math::Clamp(math::RoundToInt(rgba[2] * 255.0f), 0, 255);
}

void ConvertPixel_RGBAFloatToRGBA32(std::byte*& src, std::byte*& dst)
{
    float rgba[4];
    for (int i = 0; i != 4; ++i) {
        std::byte* pVal = (std::byte*)&rgba[i];
        *pVal++ = *src++;
        *pVal++ = *src++;
        *pVal++ = *src++;
        *pVal++ = *src++;
    }

    *dst++ = (std::byte)math::Clamp(math::RoundToInt(rgba[0] * 255.0f), 0, 255);
    *dst++ = (std::byte)math::Clamp(math::RoundToInt(rgba[1] * 255.0f), 0, 255);
    *dst++ = (std::byte)math::Clamp(math::RoundToInt(rgba[2] * 255.0f), 0, 255);
    *dst++ = (std::byte)255;
}

void ConvertPixel_RGBAFloatToRGBAFloat(std::byte*& src, std::byte*& dst)
{
    for (int i = 0; i != 4; ++i) {
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
    }
}

typedef void(*ConvertPixelFunc)(std::byte*& src, std::byte*& dst);
typedef void(*ConvertImageFunc)(std::byte* srcBegin, std::byte* srcEnd, std::byte* dstBegin);

template<ConvertPixelFunc fun>
void ConvertPixels(
    std::byte* srcBegin,
    std::byte* srcEnd,
    std::byte* dstBegin)
{
    while (srcBegin != srcEnd)
        fun(srcBegin, dstBegin);
}

ConvertImageFunc GetConvertFunc(
    PixelDataFormat srcFormat,
    PixelDataFormat dstFormat)
{
    if (srcFormat == PixelDataFormat::Alpha8)
    {
        if (dstFormat == PixelDataFormat::Alpha8 || dstFormat == PixelDataFormat::Unspecified) {
            return ConvertPixels<ConvertPixel_Alpha8ToAlpha8>;
        }
        else if (dstFormat == PixelDataFormat::RGB24) {
            return ConvertPixels<ConvertPixel_Alpha8ToRGB24>;
        }
        else if (dstFormat == PixelDataFormat::RGBA32) {
            return ConvertPixels<ConvertPixel_Alpha8ToRGBA32>;
        }
        else if (dstFormat == PixelDataFormat::RGBAFloat) {
            return ConvertPixels< ConvertPixel_Alpha8ToRGBAFloat>;
        }
    }
    else if (srcFormat == PixelDataFormat::RGB24)
    {
        if (dstFormat == PixelDataFormat::Alpha8) {
            return ConvertPixels<ConvertPixel_RGB24ToAlpha8>;
        }
        else if (dstFormat == PixelDataFormat::RGB24 || dstFormat == PixelDataFormat::Unspecified) {
            return ConvertPixels<ConvertPixel_RGB24ToRGB24>;
        }
        else if (dstFormat == PixelDataFormat::RGBA32) {
            return ConvertPixels<ConvertPixel_RGB24ToRGBA32>;
        }
        else if (dstFormat == PixelDataFormat::RGBAFloat) {
            return ConvertPixels<ConvertPixel_RGB24ToRGBAFloat>;
        }
    }
    else if (srcFormat == PixelDataFormat::RGBA32)
    {
        if (dstFormat == PixelDataFormat::Alpha8) {
            return ConvertPixels<ConvertPixel_RGBA32ToAlpha8>;
        }
        else if (dstFormat == PixelDataFormat::RGB24) {
            return ConvertPixels<ConvertPixel_RGBA32ToRGB24>;
        }
        else if (dstFormat == PixelDataFormat::RGBA32 || dstFormat == PixelDataFormat::Unspecified) {
            return ConvertPixels<ConvertPixel_RGBA32ToRGBA32>;
        }
        else if (dstFormat == PixelDataFormat::RGBAFloat) {
            return ConvertPixels<ConvertPixel_RGBA32ToRGBAFloat>;
        }
    }
    else if (srcFormat == PixelDataFormat::RGBAFloat)
    {
        if (dstFormat == PixelDataFormat::Alpha8) {
            return ConvertPixels<ConvertPixel_RGBAFloatToAlpha8>;
        }
        else if (dstFormat == PixelDataFormat::RGB24) {
            return ConvertPixels<ConvertPixel_RGBAFloatToRGB24>;
        }
        else if (dstFormat == PixelDataFormat::RGBA32) {
            return ConvertPixels<ConvertPixel_RGBAFloatToRGBA32>;
        }
        else if (dstFormat == PixelDataFormat::RGBAFloat || dstFormat == PixelDataFormat::Unspecified) {
            return ConvertPixels<ConvertPixel_RGBAFloatToRGBAFloat>;
        }
    }

    throw std::runtime_error("Invalid source or destination format");
}

void Image::Blit(
    std::byte* src, IVec2 srcSize, IntRect srcRect, PixelDataFormat srcFormat,
    std::byte* dst, IVec2 dstSize, IVec2 dstPos, PixelDataFormat dstFormat,
    bool flipVertically)
{
    if (srcFormat == PixelDataFormat::Unspecified)
        throw std::runtime_error("'srcFormat' cannot be 'Unspecified'");

    if (!src)
        throw std::runtime_error("'src' cannot be null");

    if (!dst)
        throw std::runtime_error("'dst' cannot be null");

    int srcChannels = GetBytesPerPixel(srcFormat);
    int dstChannels = GetBytesPerPixel(dstFormat);
    auto convertFunc = GetConvertFunc(srcFormat, dstFormat);

    for (int y = 0; y != srcRect.h; ++y)
    {
        auto srcX = srcRect.x;
        auto srcY = srcRect.y + y;
        auto dstX = dstPos.x;
        auto dstY = dstPos.y + y;

        if (flipVertically) {
            dstY = dstPos.y + (srcRect.h - 1 - y);
        }

        std::byte* srcBegin = &src[(srcY * srcSize.x + srcX) * srcChannels];
        std::byte* srcEnd = srcBegin + srcRect.w * srcChannels;
        std::byte* dstBegin = &dst[(dstY * dstSize.x + dstX) * dstChannels];
        convertFunc(srcBegin, srcEnd, dstBegin);
    }
}

Image Image::Clone(PixelDataFormat targetFormat)
{
    if (format == PixelDataFormat::Unspecified)
        throw std::runtime_error("source Image format cannot be 'Unspecified'");

    if (!data)
        throw std::runtime_error("source Image cannot be empty");

    auto oldByteCount = size.x * size.y * GetBytesPerPixel(format);
    auto newByteCount = size.x * size.y * GetBytesPerPixel(targetFormat);

    Image ret;
    ret.size = size;
    ret.format = targetFormat;
    ret.data = std::make_unique<std::byte[]>(newByteCount);

    std::byte* srcBegin = data.get();
    std::byte* srcEnd   = data.get() + oldByteCount;
    std::byte* dstBegin = ret.data.get();
    
    auto convertFunc = GetConvertFunc(format, targetFormat);
    convertFunc(srcBegin, srcEnd, dstBegin);

    return std::move(ret);
}

ImageFileFormat Image::GetFormatForFile(const path& filename)
{
    auto ext = ToLower(filename.extension());

    if (ext == ".tga") {
        return ImageFileFormat::TGA;
    }
    else if (ext == ".png") {
        return ImageFileFormat::PNG;
    }
    else if (ext == ".jpg") {
        return ImageFileFormat::JPG;
    }
    else if (ext == ".exr") {
        return ImageFileFormat::EXR;
    }
    else {
        throw std::runtime_error("unsupported file type");
    }
}

}
}
