/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.LineEnumerator;
import Microwave.System.Pointers;
import <string>;

export namespace mw {
inline namespace gfx {

class Font;

inline bool IsSpace(char32_t c)
{
    switch (c)
    {
    case U' ':
    case U'\t':
    case U'\n':
    case U'\v':
    case U'\f':
    case U'\r':
        return true;
    default:
        return false;
    }
}

inline bool IsCJK(char32_t code)
{
    return (code >= 0x4E00 && code <= 0x9FFF)
        || (code >= 0x3400 && code <= 0x4DBF)
        || (code >= 0x20000 && code <= 0x2A6DF)
        || (code >= 0x2A700 && code <= 0x2B73F);
}

inline bool IsHyphen(char32_t code) {
    return code == char32_t('-');
}

class LineEnumerator
{
    sptr<const Font> _font;
    std::string _input;
    int _maxLineWidth{};

    std::string_view _line;
    int _cursorIndex{};
    int _nextCursorIndex{};
    bool _overflow{};
    bool _ateLastChar{};

    typedef const char* char_ptr;
    char_ptr _begin{};
    char_ptr _end{};
    char_ptr _first{};

public:
    LineEnumerator(
        const sptr<const Font>& font,
        const std::string& input,
        int maxLineWidth = 0); // 0 = no wrapping

    int GetCursorIndex() const;
    bool DidOverflow() const;
    bool DidEatLastChar() const;
    std::string_view GetCurrentLine() const;
    bool MoveNext();
};

} // graphics
} // mw
