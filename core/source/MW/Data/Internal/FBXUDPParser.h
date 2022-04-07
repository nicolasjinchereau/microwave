/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <string>
#include <unordered_map>
#include <variant>
#include <charconv>
#include <optional>
#include <cassert>

namespace mw {
inline namespace data {

class FBXUDPMap
{
public:
    typedef std::unordered_map<
        std::string,
        std::variant<bool, float, std::string, std::vector<float>>
    > PropertyMap;

    static FBXUDPMap Parse(std::string_view input)
    {
        FBXUDPMap pmap;

        size_t lineStart = 0;
        for (;;)
        {
            size_t lineEnd = input.find('\n', lineStart);
            if (lineEnd == std::string_view::npos)
                break;

            std::string_view line = input.substr(lineStart, lineEnd - lineStart);
            lineStart = lineEnd + 1;

            auto pos = line.find('=');
            if (pos == std::string::npos)
                continue;

            std::string_view key = Trim({ line.begin(), line.begin() + pos });
            std::string_view val = Trim({ line.begin() + pos + 1, line.end() });

            if (key.empty() || val.empty())
                continue;

            if (auto boolean = ParseBoolean(val)) {
                pmap.props[std::string(key)] = boolean.value();
            }
            else if (auto arr = ParseArray(val)) {
                pmap.props[std::string(key)] = std::move(arr.value());
            }
            else if (auto number = ParseNumber(val)) {
                pmap.props[std::string(key)] = number.value();
            }
            else if (auto str = ParseString(val)) {
                pmap.props[std::string(key)] = str.value();
            }
        }

        return pmap;
    }

    template<class T>
    std::optional<T> TryGet(const std::string& key)
    {
        std::optional<T> ret;

        auto it = props.find(key);
        if (it != props.end() && std::holds_alternative<T>(it->second))
        {
            ret = std::get<T>(it->second);
        }

        return ret;
    }

private:
    PropertyMap props;

    static std::string_view Trim(std::string_view str)
    {
        if (str.empty())
            return {};

        auto left = str.begin();
        auto right = --str.end();

        while (left != right && isspace((char8_t)*left))
            ++left;

        while (right != left && isspace((char8_t)*right))
            --right;

        return std::string_view(left, right + 1);
    }

    static bool StrIEquals(std::string_view a, std::string_view b)
    {
        return a.size() == b.size() &&
            std::equal(a.begin(), a.end(), b.begin(),
                [](auto a, auto b) {
                    return std::tolower((char8_t)a) == std::tolower((char8_t)b);
                }
        );
    }

    static bool IsQuoted(std::string_view str)
    {
        return (str.size() >= 2) &&
            (str.back() == str.front()) &&
            (str.front() == '\'' || str.front() == '\"');
    }

    static std::string_view UnQuoted(std::string_view str)
    {
        assert(IsQuoted(str));
        return str.substr(1, str.size() - 2);
    }

    static std::optional<std::string> ParseString(std::string_view str)
    {
        std::optional<std::string> ret;

        if (IsQuoted(str))
            ret = UnQuoted(str);
        else
            ret = str;

        return ret;
    }

    static std::optional<std::vector<float>> ParseArray(std::string_view str)
    {
        std::optional<std::vector<float>> ret;

        if (str.front() == '[' && str.back() == ']')
        {
            std::vector<float> vals;
            
            auto contents = str.substr(1, str.size() - 2);
            auto p = contents.data();
            auto e = contents.data() + contents.size();

            while (p != e)
            {
                while (p != e && *p && (*p == ',' || isspace(*p)))
                    ++p;

                if (p == e)
                    break;

                float num;
                auto res = std::from_chars(p, e, num);

                if ((bool)res.ec)
                    return ret;

                vals.push_back(num);

                p = res.ptr;
            }

            ret = std::move(vals);
        }

        return ret;
    }

    static std::optional<float> ParseNumber(std::string_view str)
    {
        std::optional<float> ret;

        float num;
        auto res = std::from_chars(str.data(), str.data() + str.size(), num);
        if (!(bool)res.ec)
            ret = num;

        return ret;
    }

    static std::optional<bool> ParseBoolean(std::string_view str)
    {
        if (StrIEquals(str, "true")) {
            return true;
        }
        else if (StrIEquals(str, "false")) {
            return false;
        }

        return std::nullopt;
    }
};

} // data
} // mw
