/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Font;
import Microwave.Graphics.GraphicsContext;
import Microwave.Graphics.Texture;
import Microwave.Graphics.Types;
import Microwave.Math;
import Microwave.System.Object;
import Microwave.System.Pointers;
import Microwave.Utilities.BinPacking.BinPacker;
import Microwave.Utilities.BinPacking.RectMapping;
import <cstddef>;
import <cstdlib>;
import <cstdint>;
import <span>;
import <string>;
import <unordered_map>;
import <vector>;

export namespace mw {
inline namespace gfx {

class FreeTypeFontFace;

enum class FontMode
{
    Normal,
    LCD
};

struct Kerning
{
    char32_t right;
    int offset;
};

struct GlyphInfo
{
    char32_t code = {};
    int pixelHeight = {};
    int slot = {}; // bin
    int advance = {};
    int bearingX = {};
    int bearingY = {};
    int width = {};
    int height = {};
    int descent = {};
    RectMapping mapping;
    Vec3 verts[4]; // verts relative to the font's baseline and left bearing
    Vec2 tex[4]; // coordinates in the texture atlas
    //std::vector<Kerning> kerning;
};

struct LineInfo
{
    std::string_view text;
    int lineWidth = {};
    int textWidth = {};
    int spaceCount = {};
    int xStart = {};
    int spaceWidth = {};
    int extraSpaces = {};
};

class Font : public Object
{
    inline static Type::Pin<Font> pin;

    class Atlas
    {
    public:
        std::vector<uint32_t> data;
        sptr<Texture> texture;
        FontMode fontMode;

        Atlas(sptr<Texture>& texture, FontMode fontMode);
    };

public:
    typedef uint64_t GlyphKey;

    sptr<FreeTypeFontFace> fontFace;
    std::vector<GlyphInfo> glyphs;
    std::unordered_map<GlyphKey, size_t> charmap;
    BinPacker packer;
    std::vector<Atlas> atlases;

    IVec2 atlasSize = { 0, 0 };
    FontMode fontMode = FontMode::Normal;
    int pixelHeight = 0;
    int padding = 0;
    int margin = 0;
    int maxAscent = 0;
    int maxDescent = 0;
    int lineHeight = 0;
    int spaceWidth = 0;
    int tabWidth = 0;
    int spacesPerTab = 0;
    std::vector<std::byte> buffer;

    Font(std::span<std::byte> fileData,
         const IVec2& atlasSize,
         FontMode fontMode,
         int spacesPerTab,
         int padding,
         int margin);

    GlyphKey GetGlyphKey(char32_t code, int pixelHeight) const {
        return ((uint64_t)code << 32) | ((uint64_t)pixelHeight);
    }

    const GlyphInfo* GetGlyph(char32_t code) const;
    void AddCharacter(char32_t code);

    int GetAdvance(char32_t code, char32_t previous) const;

    // vertices use clockwise winding order
    void GetTextGeometry(
        const std::string& text,
        const IVec2& bounds,
        TextAlign alignment,
        int lineSpacing,
        bool wrapText,
        std::vector<UIVertex>& verts,
        std::vector<IVec2>& ranges); // x: start, y: size

    void SetPixelHeight(int pixelHeight);
private:

    LineInfo GetLineInfo(
        TextAlign alignment,
        int sizeX,
        const std::string_view& text,
        bool overflow);
};

} // gfx
} // mw
