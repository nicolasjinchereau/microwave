/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Graphics.Font;
import Microwave.Graphics.GraphicsContext;
import Microwave.Graphics.GraphicsTypes;
import Microwave.Graphics.Color32;
import Microwave.Graphics.Texture;
import Microwave.Graphics.Types;
import Microwave.Graphics.LineEnumerator;
import Microwave.Math;
import Microwave.System.App;
import Microwave.System.Pointers;
import Microwave.Utilities.BinPacking.BinPacker;
import <algorithm>;
import <cassert>;
import <cstddef>;
import <cstdlib>;
import <cstdint>;
import <span>;
import <string>;
import <stdexcept>;
import <unordered_map>;
import <utf8.h>;
import <vector>;
import <MW/Graphics/Internal/FreeType2.h>;

namespace mw {
inline namespace gfx {

class FreeTypeFontFace
{
    FT_Library library = nullptr;
    FT_Face face = nullptr;
    int pixelHeight = 0;
    std::vector<std::byte> data;

    void Cleanup()
    {
        if (face)
            FT_Done_Face(face);

        if (library)
            FT_Done_FreeType(library);
    }

public:

    FreeTypeFontFace(std::span<std::byte> fileData)
    {
        try
        {
            int ret;

            if (fileData.empty())
                throw std::runtime_error("file data is empty");

            data.reserve(fileData.size());
            data.assign(fileData.begin(), fileData.end());

            ret = FT_Init_FreeType(&library);
            if (ret != 0)
                throw std::runtime_error("failed to initialize FreeType library");

            ret = FT_New_Memory_Face(library, (FT_Byte*)data.data(), data.size(), 0, &face);
            if (ret != 0)
                throw std::runtime_error("failed to create font face");

            if (!(face->face_flags & FT_FACE_FLAG_SCALABLE) || !(face->face_flags & FT_FACE_FLAG_HORIZONTAL))
                throw std::runtime_error("font is not scaleable");
        }
        catch (...)
        {
            Cleanup();
            throw;
        }
    }

    ~FreeTypeFontFace() {
        Cleanup();
    }

    FT_Face GetFace() {
        return face;
    }

    void SetPixelHeight(int height)
    {
        assert(height > 0);

        int ret = FT_Set_Pixel_Sizes(face, 0, height);
        if (ret != 0)
            throw std::runtime_error("unsupported font size");

        this->pixelHeight = height;
    }

    int GetPixelHeight() const {
        return pixelHeight;
    }
};

bool isCJK(char32_t code)
{
    return (code >= 0x4E00 && code <= 0x9FFF)
        || (code >= 0x3400 && code <= 0x4DBF)
        || (code >= 0x20000 && code <= 0x2A6DF)
        || (code >= 0x2A700 && code <= 0x2B73F);
}

bool isHyphen(char32_t code) {
    return code == U'-';
}

static int ConvertFixedToInt(FT_Pos fixed) {
    return (int)(fixed >> 6);
}

static int GetGlyphAdvance(FT_GlyphSlot glyph) {
    return ConvertFixedToInt(glyph->metrics.horiAdvance);
}

int GetMetricFlags(FontMode mode) {
    int flags = FT_LOAD_IGNORE_TRANSFORM | FT_LOAD_NO_BITMAP;
    flags |= (mode == FontMode::Normal) ? FT_LOAD_TARGET_LIGHT : FT_LOAD_TARGET_LCD;
    return flags;
}

int GetRenderFlags(FontMode mode) {
    int flags = FT_LOAD_IGNORE_TRANSFORM | FT_LOAD_RENDER;
    flags |= (mode == FontMode::Normal) ? FT_LOAD_TARGET_LIGHT : FT_LOAD_TARGET_LCD;
    return flags;
}

Font::Atlas::Atlas(const IVec2& size, FontMode fontMode)
{
    Color32 fillColor;

    if (fontMode == FontMode::Normal)
        fillColor = Color32(255, 255, 255, 0);
    else if (fontMode == FontMode::LCD)
        fillColor = Color32(0, 0, 0, 255);
    
    std::vector<Color32> data;
    data.resize(size.x * size.y, fillColor);
    
    auto pixels = std::span<Color32>(data);
    texture = spnew<Texture>(
        std::as_writable_bytes(pixels), PixelDataFormat::RGBA32, size, true);
}

Font::Font(
    std::span<std::byte> fileData,
    const IVec2& atlasSize,
    FontMode fontMode,
    int spacesPerTab,
    int padding,
    int margin)
        : atlasSize(atlasSize),
        fontMode(fontMode),
        spacesPerTab(spacesPerTab),
        padding(padding),
        margin(margin)
{
    packer.StartDynamicPacking(atlasSize, margin, true);
    fontFace = spnew<FreeTypeFontFace>(fileData);
    
    SetPixelHeight(32);
}

void Font::SetPixelHeight(int fontPixelHeight)
{
    if (pixelHeight == fontPixelHeight)
        return;

    pixelHeight = fontPixelHeight;

    fontFace->SetPixelHeight(pixelHeight);
    buffer.resize((pixelHeight * 2) * (pixelHeight * 2) * 4);

    FT_Face face = fontFace->GetFace();

    FT_Load_Char(face, ' ', GetMetricFlags(fontMode));
    spaceWidth = face->glyph->metrics.horiAdvance >> 6;
    tabWidth = spaceWidth * spacesPerTab;

    maxAscent = face->size->metrics.ascender >> 6;
    maxDescent = abs(face->size->metrics.descender >> 6);
    lineHeight = (face->size->metrics.ascender - face->size->metrics.descender) >> 6;

    //maxAscent = 0;
    //maxDescent = 0;

    //auto sampleChars = std::span<const char32_t>(U"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
    //for (auto code : sampleChars)
    //{
    //    FT_Load_Char(face, code, GetMetricFlags(fontMode));
    //    
    //    int ascent = face->glyph->bitmap_top;
    //    int descent = face->glyph->bitmap.rows - ascent;
    //    maxAscent = std::max(ascent, maxAscent);
    //    maxDescent = std::max(descent, maxDescent);
    //}

    //lineHeight = fontFace->GetPixelHeight() + maxDescent;
}

void FillNormal(std::byte* src, std::byte* dest)
{
    *dest++ = (std::byte)255;
    *dest++ = (std::byte)255;
    *dest++ = (std::byte)255;
    *dest = *src;
};

void FillLCD(std::byte* src, std::byte* dest)
{
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src;
    *dest = std::byte(255);
};

std::byte* GetDestAddrNormal(std::byte* dst, int x, int y, int bmpWidth, int bmpHeight, int bytesPerPixel) {
    return &dst[(y * bmpWidth + x) * bytesPerPixel];
};

std::byte* GetDestAddrRotated(std::byte* dst, int x, int y, int bmpWidth, int bmpHeight, int bytesPerPixel) {
    return &dst[((bmpWidth - x - 1) * bmpHeight + y) * bytesPerPixel];
};

const GlyphInfo* Font::GetGlyph(char32_t code) const
{
    uint64_t key = GetGlyphKey(code, pixelHeight);

    auto it = charmap.find(key);
    if (it == charmap.end())
        return nullptr;

    return &glyphs[it->second];
}

void Font::AddCharacter(char32_t code)
{
    if (GetGlyph(code))
        return;

    GlyphInfo glyph;

    auto face = fontFace->GetFace();
    
    int err = FT_Load_Char(face, code, GetMetricFlags(fontMode));
    if (err != 0)
        throw std::runtime_error("Failed to load char");

    glyph.slot = 0;
    glyph.code = code;
    glyph.pixelHeight = pixelHeight;
    glyph.bearingX = face->glyph->bitmap_left;

    if (isCJK(glyph.code))
    {
        glyph.bearingY = (int)(face->glyph->metrics.vertBearingY >> 6);
        glyph.descent = 0;
    }
    else
    {
        glyph.bearingY = (int)(face->glyph->metrics.horiBearingY >> 6);
        glyph.descent = face->glyph->bitmap.rows - glyph.bearingY;
    }

    glyph.advance = GetGlyphAdvance(face->glyph);
    glyph.width = (fontMode == FontMode::LCD) ? face->glyph->bitmap.width / 3 : face->glyph->bitmap.width;
    glyph.height = face->glyph->bitmap.rows;

    // TODO: update to work with dynamic a character set
    //for(char32_t rightCode : supportedChars)
    //{
    //    int left = FT_Get_Char_Index(face, code);
    //    int right = FT_Get_Char_Index(face, rightCode);

    //    FT_Vector delta;
    //    FT_Get_Kerning(face, left, right, FT_KERNING_DEFAULT, &delta);

    //    int kerning = fixedToInt(delta.x);
    //    if(kerning != 0)
    //        glyph->kerning.push_back(Kerning{rightCode, kerning});
    //}

    glyph.mapping = packer.PackBox(IVec2(glyph.width + padding * 2, glyph.height + padding * 2));

    glyph.slot = glyph.mapping.inputIndex;
    FT_Load_Char(face, glyph.code, GetRenderFlags(fontMode));

    int dstX = glyph.mapping.mappedRect.x + padding;
    int dstY = glyph.mapping.mappedRect.y + padding;
    int bmpStride = face->glyph->bitmap.pitch;
    int bmpHeight = face->glyph->bitmap.rows;
    int bmpWidth = (fontMode == FontMode::LCD) ? face->glyph->bitmap.width / 3 : face->glyph->bitmap.width;

    // create another atlas if needed
    while ((int)atlases.size() < glyph.slot + 1)
    {
        atlases.push_back(Atlas(atlasSize, fontMode));
    }

    // blit the character to the atlas
    auto& atlas = atlases[glyph.slot];
    auto srcBytesPerPixel = (fontMode == FontMode::LCD) ? 3 : 1;
    auto bytesPerPixel = GetBytesPerPixel(atlas.texture->GetFormat());

    auto CopyPixel = (fontMode == FontMode::LCD) ? &FillLCD : &FillNormal;
    auto GetDestAddr = glyph.mapping.rotated ? &GetDestAddrRotated : &GetDestAddrNormal;

    auto src = face->glyph->bitmap.buffer;
    std::byte* dst = buffer.data();

    for (int y = 0; y < bmpHeight; ++y)
    {
        for (int x = 0; x < bmpWidth; ++x)
        {
            auto s = (std::byte*)&src[y * bmpStride + x * srcBytesPerPixel];
            auto d = GetDestAddr(dst, x, y, bmpWidth, bmpHeight, bytesPerPixel);
            CopyPixel(s, d);
        }
    }

    auto pixelData = std::span<std::byte>(buffer.data(), bmpWidth * bmpHeight * bytesPerPixel);
    auto destRect = IntRect(dstX, dstY, bmpWidth, bmpHeight);
    if (glyph.mapping.rotated) std::swap(destRect.w, destRect.h);
    atlas.texture->SetPixels(pixelData, destRect);

    // create geometry for this character
    int bx = glyph.bearingX;
    int by = glyph.bearingY;
    int cw = glyph.width;
    int ch = glyph.height;

    if (isCJK(glyph.code))
    {
        // remove "/2" ?
        int start = lineHeight - maxDescent / 2;
        glyph.verts[0] = Vec3((float)bx - padding,      (float)start - by - ch - padding, 0);
        glyph.verts[1] = Vec3((float)bx - padding,      (float)start - by + padding, 0);
        glyph.verts[2] = Vec3((float)bx + cw + padding, (float)start - by - ch - padding, 0);
        glyph.verts[3] = Vec3((float)bx + cw + padding, (float)start - by + padding, 0);
    }
    else
    {
        glyph.verts[0] = Vec3((float)bx - padding, (float)maxDescent + by - ch - padding, 0);
        glyph.verts[1] = Vec3((float)bx - padding, (float)maxDescent + by + padding, 0);
        glyph.verts[2] = Vec3((float)bx + cw + padding, (float)maxDescent + by - ch - padding, 0);
        glyph.verts[3] = Vec3((float)bx + cw + padding, (float)maxDescent + by + padding, 0);
    }

    // create texture coordinates for this character
    float tx = (float)glyph.mapping.mappedRect.x;
    float ty = (float)glyph.mapping.mappedRect.y;
    float tw = (float)atlasSize.x;
    float th = (float)atlasSize.y;

    if (glyph.mapping.rotated)
        std::swap(cw, ch);

    float u1 = tx / tw;
    float u2 = (tx + cw + padding * 2) / tw;
    float v1 = ty / th;
    float v2 = (ty + ch + padding * 2) / th;

    if (glyph.mapping.rotated)
    {
        glyph.tex[0] = Vec2(u2, v2);
        glyph.tex[1] = Vec2(u1, v2);
        glyph.tex[2] = Vec2(u2, v1);
        glyph.tex[3] = Vec2(u1, v1);
    }
    else
    {
        glyph.tex[0] = Vec2(u1, v2);
        glyph.tex[1] = Vec2(u1, v1);
        glyph.tex[2] = Vec2(u2, v2);
        glyph.tex[3] = Vec2(u2, v1);
    }

    // store the glyph
    uint64_t key = GetGlyphKey(code, pixelHeight);
    glyphs.push_back(glyph);
    charmap[key] = glyphs.size() - 1;
}

int Font::GetAdvance(char32_t code, char32_t previous) const
{
    int advance = 0;

    if (code == U' ')
        advance = spaceWidth;
    else if (code == U'\t')
        advance = tabWidth;
    else if (code == U'\n')
        advance = 0;
    else
        advance = GetGlyph(code)->advance;

    (void)previous; // use for kerning later
                    //advance += prev ? font->GetKerning(previous, code) : 0;

    return advance;
}

LineInfo Font::GetLineInfo(
    TextAlign alignment,
    int sizeX,
    const std::string_view& text,
    bool overflow)
{
    LineInfo li = {};
    li.text = text;

    char32_t prev = 0;

    for (auto it = text.begin(); it != text.end(); )
    {
        uint32_t code = utf8::next(it, text.end());
        int advance = GetAdvance(code, prev);
        prev = code;

        if (code == ' ')
            li.spaceCount += 1;
        else if (code == '\t')
            li.spaceCount += 4;
        else
            li.textWidth += advance;

        li.lineWidth += advance;
    }

    if ((int)alignment & AlignBitsLeft)
    {
        li.xStart = 0;
        li.spaceWidth = GetAdvance(' ', 0);
    }
    else if ((int)alignment & AlignBitsCenter)
    {
        li.xStart = (sizeX - li.lineWidth) / 2;
        li.spaceWidth = GetAdvance(' ', 0);
    }
    else if ((int)alignment & AlignBitsRight)
    {
        li.xStart = sizeX - li.lineWidth;
        li.spaceWidth = GetAdvance(' ', 0);
    }
    else if ((int)alignment & AlignBitsJustify)
    {
        if (overflow)
        {
            li.xStart = 0;
            li.spaceWidth = (int)((sizeX - li.textWidth) / (float)li.spaceCount);
            li.extraSpaces = (int)(sizeX - (li.spaceWidth * li.spaceCount + li.textWidth));
        }
        else
        {
            li.xStart = 0;
            li.spaceWidth = GetAdvance(' ', 0);
        }
    }

    return li;
}

void Font::GetTextGeometry(
    const std::string& text,
    const IVec2& bounds,
    TextAlign alignment,
    int lineSpacing,
    bool wrapText,
    std::vector<UIVertex>& verts,
    std::vector<IVec2>& ranges)
{
    std::vector<LineInfo> lineInfo;

    int totalWidth = 0;
    int totalHeight = 0;

    LineEnumerator lines(SharedFrom(this), text, wrapText ? bounds.x : 0);
    while (lines.MoveNext())
    {
        LineInfo li = GetLineInfo(alignment, bounds.x, lines.GetCurrentLine(), lines.DidOverflow());
        lineInfo.push_back(li);
        totalWidth = std::max(totalWidth, li.lineWidth);
    }

    int lineCount = (int)lineInfo.size();
    totalHeight = (lineCount * lineHeight) + (lineCount - 1) * lineSpacing;

    int y;

    if ((int)alignment & AlignBitsTop)
        y = bounds.y - lineHeight;
    else if ((int)alignment & AlignBitsMiddle)
        y = (bounds.y / 2) + (totalHeight / 2) - lineHeight;
    else if ((int)alignment & AlignBitsBottom)
        y = totalHeight - lineHeight;
    else
        assert(0); // no vertical alignment spacified

    // vertices per font atlas
    std::vector<std::vector<UIVertex>> vtmp;
    vtmp.resize(atlases.size());

    for (auto& li : lineInfo)
    {
        int x = li.xStart;
        char32_t prev = 0;

        for (auto it = li.text.begin(); it != li.text.end(); )
        {
            auto code = utf8::next(it, li.text.end());
            int advance = GetAdvance(code, prev);
            prev = code;

            Vec3 offset = Vec3((float)x, (float)y, 0);

            if (code == ' ')
            {
                x += li.spaceWidth;

                if (li.extraSpaces > 0)
                {
                    --li.extraSpaces;
                    x += 1;
                }
            }
            else if (code == '\t')
            {
                x += li.spaceWidth * 4;

                if (li.extraSpaces > 0)
                {
                    int spaces = std::min(li.extraSpaces, 4);
                    li.extraSpaces -= spaces;
                    x += spaces;
                }
            }
            else
            {
                x += advance;
            }

            const GlyphInfo* glyph = GetGlyph(code);

            if (glyph)
            {
                Vec3 v0 = glyph->verts[0] + offset;
                Vec3 v1 = glyph->verts[1] + offset;
                Vec3 v2 = glyph->verts[2] + offset;
                Vec3 v3 = glyph->verts[3] + offset;
                Vec2 t0 = glyph->tex[0];
                Vec2 t1 = glyph->tex[1];
                Vec2 t2 = glyph->tex[2];
                Vec2 t3 = glyph->tex[3];

                int atlas = glyph->slot;
                vtmp[atlas].push_back(UIVertex{ v0, t0, Color::White() });
                vtmp[atlas].push_back(UIVertex{ v1, t1, Color::White() });
                vtmp[atlas].push_back(UIVertex{ v2, t2, Color::White() });
                vtmp[atlas].push_back(UIVertex{ v2, t2, Color::White() });
                vtmp[atlas].push_back(UIVertex{ v1, t1, Color::White() });
                vtmp[atlas].push_back(UIVertex{ v3, t3, Color::White() });
            }
        }

        y -= (lineHeight + lineSpacing);
    }

    size_t totalCount = 0;
    for (auto& vset : vtmp)
        totalCount += vset.size();

    verts.clear();
    verts.reserve(totalCount);

    ranges.clear();
    ranges.reserve(vtmp.size());

    int start = 0;

    for (auto& vset : vtmp)
    {
        int count = (int)vset.size();
        ranges.push_back({ start, count });
        start += count;

        verts.insert(verts.end(), vset.begin(), vset.end());
    }
}

} // gfx
} // mw
