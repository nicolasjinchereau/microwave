/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/Graphics/Font.h>
#include <MW/Graphics/LineEnumerator.h>
#include <utf8.h>

namespace mw {
inline namespace gfx {

LineEnumerator::LineEnumerator(
    const std::shared_ptr<const Font>& font,
    const std::string& input,
    int maxLineWidth)
{
    _font = font;
    _input = input;
    _maxLineWidth = maxLineWidth;
    _begin = _input.c_str();
    _end = _input.c_str() + _input.size();
    _first = _begin;
}

int LineEnumerator::GetCursorIndex() const {
    return _cursorIndex;
}

bool LineEnumerator::DidOverflow() const {
    return _overflow;
}

bool LineEnumerator::DidEatLastChar() const {
    return _ateLastChar;
}

std::string_view LineEnumerator::GetCurrentLine() const {
    return _line;
}

bool LineEnumerator::MoveNext()
{
    if (_first == _end)
        return false;

    _cursorIndex = _nextCursorIndex;

    auto cur = _first;
    int lineWidth = 0;
    char32_t prev = 0;

    while (cur != _end)
    {
        char32_t code = utf8::peek_next(cur, _end);
        int advance = _font->GetAdvance(code, prev);
        prev = code;

        if (code == '\n')
        {
            _line = std::string_view(_first, cur - _first);
            _overflow = false;
            _ateLastChar = true;

            //_first = ++cur;
            utf8::next(cur, _end);
            _first = cur;

            ++_nextCursorIndex;
            return true;
        }

        if (_maxLineWidth != 0 && lineWidth + advance > _maxLineWidth)
        {
            auto br = cur;

            //while(br > _first && !IsSpace(*br) && !IsCJK(*br) && !IsHyphen(*(br - 1)))
            while (br > _first && !IsSpace(*br) && !IsCJK(*br) && !IsHyphen(utf8::peek_prior(br, _begin)))
            {
                //--br;
                utf8::prior(br, _begin);
                --_nextCursorIndex;
            }

            if (br == _first)
            {
                _line = std::string_view(_first, cur - _first);
                _first = cur;
                _ateLastChar = false;
            }
            else if (IsCJK(*br))
            {
                if (br < cur)
                {
                    //++br;
                    utf8::next(br, _end);
                    ++_nextCursorIndex;
                }

                _line = std::string_view(_first, br - _first);
                _first = cur = br;
                _ateLastChar = false;
            }
            else if (IsHyphen(utf8::peek_prior(br, _begin)))
                //else if(IsHyphen(*(br - 1)))
            {
                _line = std::string_view(_first, br - _first);
                _first = cur = br;
                _ateLastChar = false;
            }
            else // if(IsSpace(*br))
            {
                _line = std::string_view(_first, br - _first);
                utf8::next(br, _end);
                _first = cur = br;
                //_first = cur = ++br;
                _ateLastChar = true;
                ++_nextCursorIndex;
            }

            _overflow = true;
            return true;
        }
        else
        {
            utf8::next(cur, _end);
            //++cur;
            ++_nextCursorIndex;
            lineWidth += advance;
        }
    }

    _line = std::string_view(_first, cur - _first);
    _overflow = false;
    _ateLastChar = false;
    _first = _end;
    return true;
}

} // gfx
} // mw
