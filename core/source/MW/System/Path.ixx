// filesystem standard header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

export module Microwave.System.Path;
import <algorithm>;
import <iomanip>;
import <list>;
import <locale>;
import <memory>;
import <system_error>;
import <string>;
import <string_view>;
import <codecvt>;
import <filesystem>;

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
#pragma push_macro("new")
#undef new

export namespace mw {
inline namespace system {

// FUNCTION OBJECT _Is_slash
struct _Is_slash_oper { // predicate testing if input is a preferred-separator or fallback-separator
    constexpr bool operator()(
        const char _Ch) const { // test if _Ch is a preferred-separator or fallback-separator
        return _Ch == '/' || _Ch == '\\';
    }
};

inline constexpr _Is_slash_oper _Is_slash{};

template <class>
inline constexpr bool _Is_EcharT = false;
template <>
inline constexpr bool _Is_EcharT<char> = true;

template <class _Ty, class = void>
inline constexpr bool _Is_Source_impl = false;

template <class _Ty>
inline constexpr bool _Is_Source_impl<_Ty, std::void_t<typename std::iterator_traits<_Ty>::value_type>> =
    _Is_EcharT<typename std::iterator_traits<_Ty>::value_type>;

template <class _Ty>
inline constexpr bool _Is_Source = _Is_Source_impl<std::decay_t<_Ty>>;

class path;

template <>
inline constexpr bool _Is_Source<path> = false; // to avoid constraint recursion via the converting constructor and
                                                // std::iterator_traits when determining if path is copyable.

template <class _Elem, class _Traits, class _Alloc>
inline constexpr bool _Is_Source<std::basic_string<_Elem, _Traits, _Alloc>> = _Is_EcharT<_Elem>;

template <class _Elem, class _Traits>
inline constexpr bool _Is_Source<std::basic_string_view<_Elem, _Traits>> = _Is_EcharT<_Elem>;

// FUNCTION TEMPLATE _Unaligned_load
template <class _Ty>
_Ty _Unaligned_load(const void* _Ptr) { // load a _Ty from _Ptr
    static_assert(std::is_trivial_v<_Ty>, "Unaligned loads require trivial types");
    _Ty _Tmp;
    memcpy(&_Tmp, _Ptr, sizeof(_Tmp));
    return _Tmp;
}

inline bool _Is_drive_prefix(const char* const _First) {
    // test if _First points to a prefix of the form X:
    // pre: _First points to at least 2 wchar_t instances
    // pre: Little endian
    auto _Value = _Unaligned_load<unsigned int>(_First);
    _Value &= 0xFFFF'FFDFu; // transform lowercase drive letters into uppercase ones
    _Value -= (static_cast<unsigned int>(':') << (sizeof(char) * CHAR_BIT)) | 'A';
    return _Value < 26;
}

inline bool _Has_drive_letter_prefix(const char* const _First, const char* const _Last) {
    // test if [_First, _Last) has a prefix of the form X:
    return _Last - _First >= 2 && _Is_drive_prefix(_First);
}

inline const char* _Find_root_name_end(const char* const _First, const char* const _Last)
{
    // attempt to parse [_First, _Last) as a path and return the end of root-name if it exists; otherwise, _First

    // This is the place in the generic grammar where library implementations have the most freedom.
    // Below are example Windows paths, and what we've decided to do with them:
    // * X:DriveRelative, X:\DosAbsolute
    //   We parse X: as root-name, if and only if \ is present we consider that root-directory
    // * \RootRelative
    //   We parse no root-name, and \ as root-directory
    // * \\server\share
    //   We parse \\server as root-name, \ as root-directory, and share as the first element in relative-path.
    //   Technically, Windows considers all of \\server\share the logical "root", but for purposes
    //   of decomposition we want those split, so that path(R"(\\server\share)").replace_filename("other_share")
    //   is \\server\other_share
    // * \\?\device
    // * \??\device
    // * \\.\device
    //   CreateFile appears to treat these as the same thing; we will set the first three characters as root-name
    //   and the first \ as root-directory. Support for these prefixes varies by particular Windows version, but
    //   for the purposes of path decomposition we don't need to worry about that.
    // * \\?\UNC\server\share
    //   MSDN explicitly documents the \\?\UNC syntax as a special case. What actually happens is that the device
    //   Mup, or "Multiple UNC provider", owns the path \\?\UNC in the NT namespace, and is responsible for the
    //   network file access. When the user says \\server\share, CreateFile translates that into
    //   \\?\UNC\server\share to get the remote server access behavior. Because NT treats this like any other
    //   device, we have chosen to treat this as the \\?\ case above.
    if (_Last - _First < 2) {
        return _First;
    }

    if (_Has_drive_letter_prefix(_First, _Last)) { // check for X: first because it's the most common root-name
        return _First + 2;
    }

    if (!_Is_slash(_First[0])) { // all the other root-names start with a slash; check that first because
                                    // we expect paths without a leading slash to be very common
        return _First;
    }

    // $ means anything other than a slash, including potentially the end of the input
    if (_Last - _First >= 4 && _Is_slash(_First[3]) && (_Last - _First == 4 || !_Is_slash(_First[4])) // \xx\$
        && ((_Is_slash(_First[1]) && (_First[2] == '?' || _First[2] == '.')) // \\?\$ or \\.\$
            || (_First[1] == '?' && _First[2] == '?'))) { // \??\$
        return _First + 3;
    }

    if (_Last - _First >= 3 && _Is_slash(_First[1]) && !_Is_slash(_First[2])) { // \\server
        return std::find_if(_First + 3, _Last, _Is_slash);
    }

    // no match
    return _First;
}

inline std::string_view _Parse_root_name(const std::string_view _Str) {
    // attempt to parse _Str as a path and return the root-name if it exists; otherwise, an empty view
    const auto _First = _Str.data();
    const auto _Last  = _First + _Str.size();
    return std::string_view(_First, static_cast<size_t>(_Find_root_name_end(_First, _Last) - _First));
}

inline const char* _Find_relative_path(const char* const _First, const char* const _Last) {
    // attempt to parse [_First, _Last) as a path and return the start of relative-path
    return std::find_if_not(_Find_root_name_end(_First, _Last), _Last, _Is_slash);
}

inline std::string_view _Parse_root_directory(const std::string_view _Str) {
    // attempt to parse _Str as a path and return the root-directory if it exists; otherwise, an empty view
    const auto _First         = _Str.data();
    const auto _Last          = _First + _Str.size();
    const auto _Root_name_end = _Find_root_name_end(_First, _Last);
    const auto _Relative_path = std::find_if_not(_Root_name_end, _Last, _Is_slash);
    return std::string_view(_Root_name_end, static_cast<size_t>(_Relative_path - _Root_name_end));
}

inline std::string_view _Parse_root_path(const std::string_view _Str) {
    // attempt to parse _Str as a path and return the root-path if it exists; otherwise, an empty view
    const auto _First = _Str.data();
    const auto _Last  = _First + _Str.size();
    return std::string_view(_First, static_cast<size_t>(_Find_relative_path(_First, _Last) - _First));
}

inline std::string_view _Parse_relative_path(const std::string_view _Str) {
    // attempt to parse _Str as a path and return the relative-path if it exists; otherwise, an empty view
    const auto _First         = _Str.data();
    const auto _Last          = _First + _Str.size();
    const auto _Relative_path = _Find_relative_path(_First, _Last);
    return std::string_view(_Relative_path, static_cast<size_t>(_Last - _Relative_path));
}

inline std::string_view _Parse_parent_path(const std::string_view _Str) {
    // attempt to parse _Str as a path and return the parent_path if it exists; otherwise, an empty view
    const auto _First         = _Str.data();
    auto _Last                = _First + _Str.size();
    const auto _Relative_path = _Find_relative_path(_First, _Last);
    // case 1: relative-path ends in a directory-separator, remove the separator to remove "magic empty path"
    //  for example: R"(/foo/bar/\//\)"
    // case 2: relative-path doesn't end in a directory-separator, remove the filename and last directory-separator
    //  to prevent creation of a "magic empty path"
    //  for example: "/foo/bar"
    while (_Relative_path != _Last && !_Is_slash(_Last[-1])) {
        // handle case 2 by removing trailing filename, puts us into case 1
        --_Last;
    }

    while (_Relative_path != _Last && _Is_slash(_Last[-1])) { // handle case 1 by removing trailing slashes
        --_Last;
    }

    return std::string_view(_First, static_cast<size_t>(_Last - _First));
}

inline const char* _Find_filename(const char* const _First, const char* _Last) {
    // attempt to parse [_First, _Last) as a path and return the start of filename if it exists; otherwise, _Last
    const auto _Relative_path = _Find_relative_path(_First, _Last);
    while (_Relative_path != _Last && !_Is_slash(_Last[-1])) {
        --_Last;
    }

    return _Last;
}

inline std::string_view _Parse_filename(const std::string_view _Str) {
    // attempt to parse _Str as a path and return the filename if it exists; otherwise, an empty view
    const auto _First    = _Str.data();
    const auto _Last     = _First + _Str.size();
    const auto _Filename = _Find_filename(_First, _Last);
    return std::string_view(_Filename, static_cast<size_t>(_Last - _Filename));
}

constexpr const char* _Find_extension(const char* const _Filename, const char* const _Ads) {
    // find dividing point between stem and extension in a generic format filename consisting of [_Filename, _Ads)
    auto _Extension = _Ads;
    if (_Filename == _Extension) { // empty path
        return _Ads;
    }

    --_Extension;
    if (_Filename == _Extension) {
        // path is length 1 and either dot, or has no dots; either way, extension() is empty
        return _Ads;
    }

    if (*_Extension == '.') { // we might have found the end of stem
        if (_Filename == _Extension - 1 && _Extension[-1] == '.') { // dotdot special case
            return _Ads;
        } else { // x.
            return _Extension;
        }
    }

    while (_Filename != --_Extension) {
        if (*_Extension == '.') { // found a dot which is not in first position, so it starts extension()
            return _Extension;
        }
    }

    // if we got here, either there are no dots, in which case extension is empty, or the first element
    // is a dot, in which case we have the leading single dot special case, which also makes extension empty
    return _Ads;
}

inline std::string_view _Parse_stem(const std::string_view _Str) {
    // attempt to parse _Str as a path and return the stem if it exists; otherwise, an empty view
    const auto _First    = _Str.data();
    const auto _Last     = _First + _Str.size();
    const auto _Filename = _Find_filename(_First, _Last);
    const auto _Ads =
        std::find(_Filename, _Last, ':'); // strip alternate data streams in intra-filename decomposition
    const auto _Extension = _Find_extension(_Filename, _Ads);
    return std::string_view(_Filename, static_cast<size_t>(_Extension - _Filename));
}

inline std::string_view _Parse_extension(const std::string_view _Str) {
    // attempt to parse _Str as a path and return the extension if it exists; otherwise, an empty view
    const auto _First    = _Str.data();
    const auto _Last     = _First + _Str.size();
    const auto _Filename = _Find_filename(_First, _Last);
    const auto _Ads = std::find(_Filename, _Last, ':'); // strip alternate data streams in intra-filename decomposition
    const auto _Extension = _Find_extension(_Filename, _Ads);
    return std::string_view(_Extension, static_cast<size_t>(_Ads - _Extension));
}

inline int _Range_compare(
    const char* const _Lfirst, const char* const _Llast,
    const char* const _Rfirst, const char* const _Rlast) {
    // 3 way compare [_Lfirst, _Llast) with [_Rfirst, _Rlast)

    std::string_view left(_Lfirst, _Llast - _Lfirst);
    std::string_view right(_Rfirst, _Rlast - _Rfirst);
    return left.compare(right);
}

constexpr bool _Starts_with(const std::string_view str, const std::string_view substr) noexcept
{
    return str.size() >= substr.size() && std::char_traits<char>::compare(str.data(), substr.data(), substr.size()) == 0;
}

inline bool _Is_drive_prefix_with_slash_slash_question(const std::string_view _Text) {
    // test if _Text starts with a \\?\X: prefix
    using namespace std::string_view_literals; // TRANSITION, VSO-571749
    return _Text.size() >= 6 && _Starts_with(_Text, R"(\\?\)"sv) && _Is_drive_prefix(_Text.data() + 4);
}

template <class _Base_iter>
class _Path_iterator;

class path
{
    template <class _Base_iter>
    friend class _Path_iterator;

    template <class _Iter>
    using _Iter_value_t = typename std::iterator_traits<_Iter>::value_type;

public:
    using value_type  = char;
    using string_type = std::string;

    static constexpr char preferred_separator = '/';

    enum format { auto_format, native_format, generic_format };

    path()            = default;
    path(const path&) = default;
    path(path&&)      = default;
    ~path()           = default;
    path& operator=(const path&) = default;
    path& operator=(path&&) noexcept = default;

    path(string_type&& _Source) : _Text(std::move(_Source)) {}

    path(string_type&& _Source, format) : _Text(std::move(_Source)) {
        // format has no meaning for this implementation, as the generic grammar is acceptable as a native path
    }

    template <class _Src, std::enable_if_t<_Is_Source<_Src>, int> = 0>
    path(const _Src& _Source, format = auto_format) : _Text(_Source) {
        // format has no meaning for this implementation, as the generic grammar is acceptable as a native path
    }

    template <class _InIt>
    path(_InIt _First, _InIt _Last, format = auto_format) : _Text(_First, _Last) {
        // format has no meaning for this implementation, as the generic grammar is acceptable as a native path
        static_assert(_Is_EcharT<_Iter_value_t<_InIt>>, "invalid value_type, see N4810 29.11.4 [fs.req]/3");
    }

    path(const std::filesystem::path& p)
        : _Text(p.string())
    {
        make_preferred();
    }

    path& operator=(string_type&& _Source) noexcept /* strengthened */ {
        // set native() to _Source
        _Text = std::move(_Source);
        return *this;
    }

    path& assign(string_type&& _Source) noexcept /* strengthened */ {
        // set native() to _Source
        _Text = std::move(_Source);
        return *this;
    }

    template <class _Src, std::enable_if_t<_Is_Source<_Src>, int> = 0>
    path& operator=(const _Src& _Source) { // set native() to _Source
        _Text = _Source;
        return *this;
    }

    template <class _Src, std::enable_if_t<_Is_Source<_Src>, int> = 0>
    path& assign(const _Src& _Source) { // set native() to _Source
        _Text = _Source;
        return *this;
    }

    template <class _InIt>
    path& assign(_InIt _First, _InIt _Last) { // set native() to [_First, _Last)
        static_assert(_Is_EcharT<_Iter_value_t<_InIt>>, "invalid value_type, see N4810 29.11.4 [fs.req]/3");
        _Text.assign(_First, _Last);
        return *this;
    }

    path& operator/=(const path& _Other) {
        // set *this to the path lexically resolved by _Other relative to *this
        // examples:
        // path("foo") / "c:/bar"; // yields "c:/bar"
        // path("foo") / "c:"; // yields "c:"
        // path("c:") / ""; // yields "c:"
        // path("c:foo") / "/bar"; // yields "c:/bar"
        // path("c:foo") / "c:bar"; // yields "c:foo/bar"
        // path("c:foo") / "d:bar"; // yields "d:bar"
        // several places herein quote the standard, but the standard's variable p is replaced with _Other

        if (_Other.is_absolute()) { // if _Other.is_absolute(), then op=(_Other)
            return operator=(_Other);
        }

        const auto _My_first            = _Text.data();
        const auto _My_last             = _My_first + _Text.size();
        const auto _Other_first         = _Other._Text.data();
        const auto _Other_last          = _Other_first + _Other._Text.size();
        const auto _My_root_name_end    = _Find_root_name_end(_My_first, _My_last);
        const auto _Other_root_name_end = _Find_root_name_end(_Other_first, _Other_last);
        if (_Other_first != _Other_root_name_end
            && _Range_compare(_My_first, _My_root_name_end, _Other_first, _Other_root_name_end) != 0) {
            // if _Other.has_root_name() && _Other.root_name() != root_name(), then op=(_Other)
            return operator=(_Other);
        }

        if (_Other_root_name_end != _Other_last && _Is_slash(*_Other_root_name_end)) {
            // If _Other.has_root_directory() removes any root directory and relative-path from *this
            _Text.erase(static_cast<size_t>(_My_root_name_end - _My_first));
        } else {
            // Otherwise, if (!has_root_directory && is_absolute) || has_filename appends path::preferred_separator
            if (_My_root_name_end == _My_last) {
                // Here, !has_root_directory && !has_filename
                // Going through our root_name kinds:
                // X: can't be absolute here, since those paths are absolute only when has_root_directory
                // \\?\ can't exist without has_root_directory
                // \\server can be absolute here
                if (_My_root_name_end - _My_first >= 3) {
                    _Text.push_back(preferred_separator);
                }
            } else {
                // Here, has_root_directory || has_filename
                // If there is a trailing slash, the trailing slash might be part of root_directory.
                // If it is, has_root_directory && !has_filename, so the test fails.
                // If there is a trailing slash not part of root_directory, then !has_filename, so only
                // (!has_root_directory && is_absolute) remains
                // Going through our root_name kinds:
                // X:foo\ needs a root_directory to be absolute
                // \\server\foo must have a root_directory to exist with a relative_path
                // \\?\ must have a root_directory to exist
                // As a result, the test fails if there is a trailing slash.
                // If there is no trailing slash, then has_filename, so the test passes.
                // Therefore, the test passes if and only if there is no trailing slash.
                if (!_Is_slash(_My_last[-1])) {
                    _Text.push_back(preferred_separator);
                }
            }
        }

        // Then appends the native format pathname of _Other, omitting any root-name from its generic format
        // pathname, to the native format pathname.
        _Text.append(_Other_root_name_end, static_cast<size_t>(_Other_last - _Other_root_name_end));
        return *this;
    }

    template <class _Src, std::enable_if_t<_Is_Source<_Src>, int> = 0>
    path& operator/=(const _Src& _Source) {
        return operator/=(path(_Source));
    }

    template <class _Src, std::enable_if_t<_Is_Source<_Src>, int> = 0>
    path& append(const _Src& _Source) {
        return operator/=(path(_Source));
    }

    template <class _InIt>
    path& append(_InIt _First, _InIt _Last) {
        static_assert(_Is_EcharT<_Iter_value_t<_InIt>>, "invalid value_type, see N4810 29.11.4 [fs.req]/3");
        return operator/=(path(_First, _Last));
    }

    path& operator+=(const path& _Added) { // concat _Added to native()
        _Text += _Added._Text;
        return *this;
    }

    path& operator+=(const string_type& _Added) { // concat _Added to native()
        _Text += _Added;
        return *this;
    }

    path& operator+=(const std::string_view _Added) { // concat _Added to native()
        _Text += _Added;
        return *this;
    }

    path& operator+=(const value_type* const _Added) { // concat _Added to native()
        _Text += _Added;
        return *this;
    }

    path& operator+=(const value_type _Added) { // concat _Added to native()
        _Text += _Added;
        return *this;
    }

    template <class _Src, std::enable_if_t<_Is_Source<_Src>, int> = 0>
    path& operator+=(const _Src& _Added) { // concat _Added to native()
        return operator+=(path(_Added));
    }

    template <class _EcharT, std::enable_if_t<_Is_EcharT<_EcharT>, int> = 0>
    path& operator+=(const _EcharT _Added) { // concat _Added to native()
        return operator+=(path(&_Added, &_Added + 1));
    }

    template <class _Src, std::enable_if_t<_Is_Source<_Src>, int> = 0>
    path& concat(const _Src& _Added) { // concat _Added to native()
        return operator+=(path(_Added));
    }

    template <class _InIt>
    path& concat(_InIt _First, _InIt _Last) { // concat [_First, _Last) to native()
        static_assert(_Is_EcharT<_Iter_value_t<_InIt>>, "invalid value_type, see N4810 29.11.4 [fs.req]/3");
        return operator+=(path(_First, _Last));
    }

    void clear() noexcept { // set *this to the empty path
        _Text.clear();
    }

    path& make_preferred() noexcept /* strengthened */ {
        // transform each fallback-separator into preferred-separator
        std::replace(_Text.begin(), _Text.end(), '\\', '/');
        return *this;
    }

    path& remove_filename() noexcept /* strengthened */ {
        // remove any filename from *this
        const auto _First    = _Text.data();
        const auto _Last     = _First + _Text.size();
        const auto _Filename = _Find_filename(_First, _Last);
        _Text.erase(static_cast<size_t>(_Filename - _First));
        return *this;
    }

    void _Remove_filename_and_separator() noexcept { // remove filename and preceding non-root directory-separator
        const auto _First         = _Text.data();
        const auto _Last          = _First + _Text.size();
        const auto _Root_name_end = _Find_root_name_end(_First, _Last);
        const auto _Root_dir_end =
            (_Root_name_end != _Last && _Is_slash(*_Root_name_end)) ? _Root_name_end + 1 : _Root_name_end;

        using _Reverse_iter = std::reverse_iterator<const char*>;

        const _Reverse_iter _Rbegin{_Last};
        const _Reverse_iter _Rend{_Root_dir_end};

        const auto _Rslash_first = std::find_if(_Rbegin, _Rend, _Is_slash);
        const auto _Rslash_last  = std::find_if_not(_Rslash_first, _Rend, _Is_slash);

        const _Reverse_iter _Rlast{_First};

        _Text.erase(static_cast<size_t>(_Rlast - _Rslash_last));
    }

    path& replace_filename(const path& _Replacement) { // remove any filename from *this and append _Replacement
        remove_filename();
        return operator/=(_Replacement);
    }

    path& replace_extension(/* const path& _Replacement = path() */) noexcept /* strengthened */ {
        // remove any extension() (and alternate data stream references) from *this's filename()
        const char* _First    = _Text.data();
        const auto _Last      = _First + _Text.size();
        const auto _Filename  = _Find_filename(_First, _Last);
        const auto _Ads       = std::find(_Filename, _Last, L':');
        const auto _Extension = _Find_extension(_Filename, _Ads);
        _Text.erase(static_cast<size_t>(_Extension - _First));
        return *this;
    }

    path& replace_extension(const path& _Replacement) {
        // remove any extension() (and alternate data stream references) from *this's filename(), and concatenate
        // _Replacement
        replace_extension();
        if (!_Replacement.empty() && _Replacement._Text[0] != '.') {
            _Text.push_back(L'.');
        }

        return operator+=(_Replacement);
    }

    void swap(path& _Rhs) noexcept {
        _Text.swap(_Rhs._Text);
    }

    const string_type& native() const noexcept {
        // return a reference to the internally stored std::string in the native format
        return _Text;
    }

    const value_type* c_str() const noexcept {
        // return a NTCTS to the internally stored path in the native format
        return _Text.c_str();
    }

    operator const string_type&() const { // implicitly convert *this into a string containing the native format
        return _Text;
    }

    const std::string& string() const { // convert the native path from this instance into a string
        return _Text;
    }

    std::string generic_string() const {
        auto ret = string();
        std::replace(ret.begin(), ret.end(), '\\', '/');
        return ret;
    }

    std::filesystem::path std_path() const {
        std::filesystem::path ret = _Text;
        ret.make_preferred();
        return ret;
    }

    int compare(const path& _Other) const noexcept { // compare *this with _Other
        return compare(static_cast<std::string_view>(_Other._Text));
    }

    int compare(const string_type& _Other) const noexcept /* strengthened */ {
        // compare *this with _Other
        return compare(static_cast<std::string_view>(_Other));
    }

    int compare(const std::basic_string_view<value_type> _Other) const noexcept /* strengthened */ {
        // compare *this with _Other
        // several places herein quote the standard, but the standard's variable p is replaced with _Other
        const auto _My_first            = _Text.data();
        const auto _My_last             = _My_first + _Text.size();
        const auto _My_root_name_end    = _Find_root_name_end(_My_first, _My_last);
        const auto _Other_first         = _Other.data();
        const auto _Other_last          = _Other_first + _Other.size();
        const auto _Other_root_name_end = _Find_root_name_end(_Other_first, _Other_last);

        // Let rootNameComparison be the result of this->root_name().native().compare(_Other.root_name().native())
        const int _Root_cmp = _Range_compare(_My_first, _My_root_name_end, _Other_first, _Other_root_name_end);
        if (_Root_cmp != 0) { // If rootNameComparison is not 0, rootNameComparison
            return _Root_cmp;
        }

        auto _My_relative               = std::find_if_not(_My_root_name_end, _My_last, _Is_slash);
        auto _Other_relative            = std::find_if_not(_Other_root_name_end, _Other_last, _Is_slash);
        const bool _My_has_root_name    = _My_root_name_end != _My_relative;
        const bool _Other_has_root_name = _Other_root_name_end != _Other_relative;
        // If !this->has_root_directory() and _Other.has_root_directory(), a value less than 0
        // If this->has_root_directory() and !_Other.has_root_directory(), a value greater than 0
        const int _Root_name_cmp = _My_has_root_name - _Other_has_root_name;
        if (_Root_name_cmp != 0) {
            return _Root_name_cmp;
        }

        // Otherwise, lexicographic by element
        for (;;) {
            const bool _My_empty    = _My_relative == _My_last;
            const bool _Other_empty = _Other_relative == _Other_last;
            const int _Empty_cmp    = _Other_empty - _My_empty;
            if (_My_empty || _Empty_cmp != 0) {
                return _Empty_cmp;
            }

            const bool _My_slash    = _Is_slash(*_My_relative);
            const bool _Other_slash = _Is_slash(*_Other_relative);
            const int _Slash_cmp    = _Other_slash - _My_slash;
            if (_Slash_cmp != 0) {
                return _Slash_cmp;
            }

            if (_My_slash) { // comparing directory-separator
                _My_relative    = std::find_if_not(_My_relative + 1, _My_last, _Is_slash);
                _Other_relative = std::find_if_not(_Other_relative + 1, _Other_last, _Is_slash);
                continue;
            }

            const int _Cmp = *_My_relative - *_Other_relative;
            if (_Cmp != 0) {
                return _Cmp;
            }

            ++_My_relative;
            ++_Other_relative;
        }
    }

    int compare(const value_type* const _Other) const noexcept /* strengthened */ {
        // compare *this with _Other
        return compare(static_cast<std::string_view>(_Other));
    }

    path root_name() const {
        // parse the root-name from *this and return a copy if present; otherwise, return the empty path
        return _Parse_root_name(_Text);
    }

    path root_directory() const {
        // parse the root-directory from *this and return a copy if present; otherwise, return the empty path
        return _Parse_root_directory(_Text);
    }

    path root_path() const {
        // parse the root-path from *this and return a copy if present; otherwise, return the empty path
        return _Parse_root_path(_Text);
    }

    path relative_path() const {
        // parse the relative-path from *this and return a copy if present; otherwise, return the empty path
        return _Parse_relative_path(_Text);
    }

    path parent_path() const {
        // parse the parent-path from *this and return a copy if present; otherwise, return the empty path
        return _Parse_parent_path(_Text);
    }

    std::string_view filename() const {
        // parse the filename from *this and return a copy if present; otherwise, return the empty path
        return _Parse_filename(_Text);
    }

    path stem() const {
        // parse the stem from *this and return a copy if present; otherwise, return the empty path
        return _Parse_stem(_Text);
    }

    path extension() const {
        // parse the extension from *this and return a copy if present; otherwise, return the empty path
        return _Parse_extension(_Text);
    }

    bool empty() const noexcept {
        return _Text.empty();
    }

    bool has_root_name() const noexcept /* strengthened */ {
        // parse the root-name from *this and return whether it exists
        return !_Parse_root_name(_Text).empty();
    }

    bool has_root_directory() const noexcept /* strengthened */ {
        // parse the root-directory from *this and return whether it exists
        return !_Parse_root_directory(_Text).empty();
    }

    bool has_root_path() const noexcept /* strengthened */ {
        // parse the root-path from *this and return whether it exists
        return !_Parse_root_path(_Text).empty();
    }

    bool has_relative_path() const noexcept /* strengthened */ {
        // parse the relative-path from *this and return whether it exists
        return !_Parse_relative_path(_Text).empty();
    }

    bool has_parent_path() const noexcept /* strengthened */ {
        // parse the parent-path from *this and return whether it exists
        return !_Parse_parent_path(_Text).empty();
    }

    bool has_filename() const noexcept /* strengthened */ {
        // parse the filename from *this and return whether it exists
        return !_Parse_filename(_Text).empty();
    }

    bool has_stem() const noexcept /* strengthened */ {
        // parse the stem from *this and return whether it exists
        return !_Parse_stem(_Text).empty();
    }

    bool has_extension() const noexcept /* strengthened */ {
        // parse the extension from *this and return whether it exists
        return !_Parse_extension(_Text).empty();
    }

    bool is_absolute() const noexcept /* strengthened */ {
        // paths with a root-name that is a drive letter and no root-directory are drive relative, such as x:example
        // paths with no root-name or root-directory are relative, such as example
        // paths with no root-name but a root-directory are root relative, such as \example
        // all other paths are absolute
        const auto _First = _Text.data();
        const auto _Last  = _First + _Text.size();
        if (_Has_drive_letter_prefix(_First, _Last)) { // test for X:\ but not X:foo
            return _Last - _First >= 3 && _Is_slash(_First[2]);
        }

        // if root-name is otherwise nonempty, then it must be one of the always-absolute prefixes like
        // \\?\ or \\server, so the path is absolute. Otherwise it is relative.
        return _First != _Find_root_name_end(_First, _Last);
    }

    bool is_relative() const noexcept /* strengthened */ {
        // test if *this is a relative path
        return !is_absolute();
    }

    path lexically_normal() const {
        using namespace std::string_view_literals; // TRANSITION, VSO-571749

        constexpr std::string_view _Dot     = "."sv;
        constexpr std::string_view _Dot_dot = ".."sv;

        // N4810 29.11.7.1 [fs.path.generic]/6:
        // "Normalization of a generic format pathname means:"

        // "1. If the path is empty, stop."
        if (empty()) {
            return {};
        }

        // "2. Replace each slash character in the root-name with a preferred-separator."
        const auto _First         = _Text.data();
        const auto _Last          = _First + _Text.size();
        const auto _Root_name_end = _Find_root_name_end(_First, _Last);
        string_type _Normalized(_First, _Root_name_end);
        std::replace(_Normalized.begin(), _Normalized.end(), '\\', '/');

        // "3. Replace each directory-separator with a preferred-separator.
        // [ Note: The generic pathname grammar (29.11.7.1) defines directory-separator
        // as one or more slashes and preferred-separators. -end note ]"
        std::list<std::string_view> _Lst; // Empty wstring_view means directory-separator
                                    // that will be normalized to a preferred-separator.
                                    // Non-empty wstring_view means filename.
        for (auto _Next = _Root_name_end; _Next != _Last;) {
            if (_Is_slash(*_Next)) {
                if (_Lst.empty() || !_Lst.back().empty()) {
                    // collapse one or more slashes and preferred-separators to one empty wstring_view
                    _Lst.emplace_back();
                }

                ++_Next;
            } else {
                const auto _Filename_end = std::find_if(_Next + 1, _Last, _Is_slash);
                _Lst.emplace_back(_Next, static_cast<size_t>(_Filename_end - _Next));
                _Next = _Filename_end;
            }
        }

        // "4. Remove each dot filename and any immediately following directory-separator."
        for (auto _Next = _Lst.begin(); _Next != _Lst.end();) {
            if (*_Next == _Dot) {
                _Next = _Lst.erase(_Next); // erase dot filename

                if (_Next != _Lst.end()) {
                    _Next = _Lst.erase(_Next); // erase immediately following directory-separator
                }
            } else {
                ++_Next;
            }
        }

        // "5. As long as any appear, remove a non-dot-dot filename immediately followed by a
        // directory-separator and a dot-dot filename, along with any immediately following directory-separator."
        for (auto _Next = _Lst.begin(); _Next != _Lst.end();) {
            auto _Prev = _Next;

            ++_Next; // If we aren't going to erase, keep advancing.
                        // If we're going to erase, _Next now points past the dot-dot filename.

            if (*_Prev == _Dot_dot && _Prev != _Lst.begin() && --_Prev != _Lst.begin() && *--_Prev != _Dot_dot) {
                if (_Next != _Lst.end()) { // dot-dot filename has an immediately following directory-separator
                    ++_Next;
                }

                _Lst.erase(_Prev, _Next); // _Next remains valid
            }
        }

        // "6. If there is a root-directory, remove all dot-dot filenames
        // and any directory-separators immediately following them.
        // [ Note: These dot-dot filenames attempt to refer to nonexistent parent directories. -end note ]"
        if (!_Lst.empty() && _Lst.front().empty()) { // we have a root-directory
            for (auto _Next = _Lst.begin(); _Next != _Lst.end();) {
                if (*_Next == _Dot_dot) {
                    _Next = _Lst.erase(_Next); // erase dot-dot filename

                    if (_Next != _Lst.end()) {
                        _Next = _Lst.erase(_Next); // erase immediately following directory-separator
                    }
                } else {
                    ++_Next;
                }
            }
        }

        // "7. If the last filename is dot-dot, remove any trailing directory-separator."
        if (_Lst.size() >= 2 && _Lst.back().empty() && *(std::prev(_Lst.end(), 2)) == _Dot_dot) {
            _Lst.pop_back();
        }

        // Build up _Normalized by flattening _Lst.
        for (const auto& _Elem : _Lst) {
            if (_Elem.empty()) {
                _Normalized += preferred_separator;
            } else {
                _Normalized += _Elem;
            }
        }

        // "8. If the path is empty, add a dot."
        if (_Normalized.empty()) {
            _Normalized = _Dot;
        }

        // "The result of normalization is a path in normal form, which is said to be normalized."
        path _Result(std::move(_Normalized));

        return _Result;
    }

    inline path lexically_relative(const path& _Base) const;

    path lexically_proximate(const path& _Base) const {
        path _Result = lexically_relative(_Base);

        if (_Result.empty()) {
            _Result = *this;
        }

        return _Result;
    }

    using iterator       = _Path_iterator<string_type::const_iterator>;
    using const_iterator = iterator;

    inline iterator begin() const;
    inline iterator end() const noexcept; // strengthened

    template <class _Elem, class _Traits>
    friend std::basic_ostream<_Elem, _Traits>& operator<<( // TRANSITION, VSO-570323
        std::basic_ostream<_Elem, _Traits>& _Ostr, const path& _Path) { // TRANSITION, VSO-570323
        // insert a path into a stream
        return _Ostr << std::quoted(_Path.string());
    }

    template <class _Elem, class _Traits>
    friend std::basic_istream<_Elem, _Traits>& operator>>( // TRANSITION, VSO-570323
        std::basic_istream<_Elem, _Traits>& _Istr, path& _Path) { // TRANSITION, VSO-570323
        // extract a path from a stream
        std::basic_string<_Elem, _Traits> _Tmp;
        _Istr >> std::quoted(_Tmp);
        _Path = std::move(_Tmp); // obvious optimization not depicted in N4810 29.11.7.6 [fs.path.io]/3
        return _Istr;
    }

    friend bool operator==(const path& _Left, const path& _Right) noexcept {
        return _Left.compare(_Right) == 0;
    }

    friend bool operator!=(const path& _Left, const path& _Right) noexcept {
        return _Left.compare(_Right) != 0;
    }

    friend bool operator<(const path& _Left, const path& _Right) noexcept {
        return _Left.compare(_Right) < 0;
    }

    friend bool operator>(const path& _Left, const path& _Right) noexcept {
        return _Left.compare(_Right) > 0;
    }

    friend bool operator<=(const path& _Left, const path& _Right) noexcept {
        return _Left.compare(_Right) <= 0;
    }

    friend bool operator>=(const path& _Left, const path& _Right) noexcept {
        return _Left.compare(_Right) >= 0;
    }

    friend path operator/(const path& _Left, const path& _Right) { // append a pair of paths together
        return path(_Left) /= _Right;
    }

    // filesystem operations
    static bool exists(const path& p) {
        return std::filesystem::exists(p._Text);
    }

    static bool exists(const path& p, std::error_code& ec) {
        return std::filesystem::exists(p._Text, ec);
    }

    static bool is_directory(const path& p) {
        return std::filesystem::is_directory(p._Text);
    }

    static bool is_directory(const path& p, std::error_code& ec) {
        return std::filesystem::is_directory(p._Text, ec);
    }

    static bool remove(const path& p) {
        return std::filesystem::remove(p._Text);
    }

    static bool remove(const path& p, std::error_code& ec) noexcept {
        return std::filesystem::remove(p._Text, ec);
    }

    static path relative(const path& p, const path& base) {
        path ret = std::filesystem::relative(p._Text, base._Text).string();
        ret.make_preferred();
        return ret;
    }

    static bool is_empty(const path& p) {
        return std::filesystem::is_empty(p._Text);
    }

    static bool is_empty(const path& p, std::error_code& ec) {
        return std::filesystem::is_empty(p._Text, ec);
    }

    typedef std::filesystem::copy_options copy_options;

    static void copy(
        const path& from,
        const path& to,
        const path::copy_options options,
        std::error_code ec)
    {
        std::filesystem::copy(from._Text, to._Text, options, ec);
    }

    static void copy(
        const path& from,
        const path& to,
        const path::copy_options options)
    {
        std::filesystem::copy(from._Text, to._Text, options);
    }

    static void copy(
        const path& from,
        const path& to,
        std::error_code ec)
    {
        std::filesystem::copy(from._Text, to._Text, ec);
    }

    static void copy(
        const path& from,
        const path& to)
    {
        std::filesystem::copy(from._Text, to._Text);
    }

    static bool create_directories(const path& p, std::error_code& ec) {
        return std::filesystem::create_directories(p._Text, ec);
    }

    static bool create_directories(const path& p) {
        return std::filesystem::create_directories(p._Text);
    }

    static std::filesystem::file_time_type last_write_time(
        const path& p)
    {
        return std::filesystem::last_write_time(p._Text);
    }

    static std::filesystem::file_time_type last_write_time(
        const path& p, std::error_code& ec)
    {
        return std::filesystem::last_write_time(p._Text, ec);
    }

    static void last_write_time(
        const path& p, std::filesystem::file_time_type newTime)
    {
        return std::filesystem::last_write_time(p._Text, newTime);
    }

    static void last_write_time(
        const path& p, std::filesystem::file_time_type newTime, std::error_code& ec)
    {
        return std::filesystem::last_write_time(p._Text, newTime, ec);
    }
private:
    string_type _Text;
};

inline std::string operator+(const char* left, const path& right) {
    return left + right.string();
}

inline std::string operator+(const std::string& left, const path& right) {
    return left + right.string();
}

template <class _Iter, class = void>
inline constexpr bool _Allow_inheriting_unwrap_v = true;

template <class _Iter>
inline constexpr bool _Allow_inheriting_unwrap_v<_Iter, std::void_t<typename _Iter::_Prevent_inheriting_unwrap>> =
    std::is_same_v<_Iter, typename _Iter::_Prevent_inheriting_unwrap>;

template <class _Ty>
using _Remove_cvref_t = std::remove_cv_t<std::remove_reference_t<_Ty>>;

template <class _Iter, class = void>
inline constexpr bool _Unwrappable_v = false;

template <class _Iter>
inline constexpr bool _Unwrappable_v<_Iter,
    std::void_t<decltype(std::declval<_Remove_cvref_t<_Iter>&>()._Seek_to(std::declval<_Iter>()._Unwrapped()))>> =
    _Allow_inheriting_unwrap_v<_Remove_cvref_t<_Iter>>;

template <class _Iter>
constexpr decltype(auto) __Get_unwrapped(_Iter&& _It)
{
    if constexpr (std::is_pointer_v<std::decay_t<_Iter>>) { // special-case pointers and arrays
        return _It + 0;
    }
    else if constexpr (_Unwrappable_v<_Iter>) {
        return static_cast<_Iter&&>(_It)._Unwrapped();
    }
    else {
        return static_cast<_Iter&&>(_It);
    }
}

template <class _Iter>
using _Unwrapped_t = _Remove_cvref_t<decltype(__Get_unwrapped(std::declval<_Iter>()))>;

// FUNCTION TEMPLATE _Get_unwrapped_unverified
template <class _Iter, class = bool>
inline constexpr bool _Do_unwrap_when_unverified_v = false;

template <class _Iter>
inline constexpr bool
    _Do_unwrap_when_unverified_v<_Iter, decltype(static_cast<bool>(_Iter::_Unwrap_when_unverified))> =
    static_cast<bool>(_Iter::_Unwrap_when_unverified);

// FUNCTION TEMPLATE _Seek_wrapped
template <class _Iter, class _UIter, class = void>
inline constexpr bool __Wrapped_seekable_v = false;

template <class _Iter, class _UIter>
inline constexpr bool
    __Wrapped_seekable_v<_Iter, _UIter, std::void_t<decltype(std::declval<_Iter&>()._Seek_to(std::declval<_UIter>()))>> =
    true;

template <class _Iter, class _UIter>
constexpr void __Seek_wrapped(_Iter& _It, _UIter&& _UIt) {
    if constexpr (__Wrapped_seekable_v<_Iter, _UIter>) {
        _It._Seek_to(static_cast<_UIter&&>(_UIt));
    } else {
        _It = static_cast<_UIter&&>(_UIt);
    }
}

// CLASS TEMPLATE _Path_iterator
template <class _Base_iter>
class _Path_iterator { // almost bidirectional iterator for path
public:
    // Note that the path::iterator can be decremented and can be dereferenced multiple times,
    // but doesn't actually meet the forward iterator requirements. See N4810 29.11.7.5 [fs.path.itr]/2:
    //  ... for dereferenceable iterators a and b of type path::iterator with a == b, there is no requirement
    //  that *a and *b are bound to the same object
    // This means that you can't give path::iterator to std::reverse_iterator, as operator* there ends
    // up returning a reference to a destroyed temporary. Other algorithms requiring bidirectional
    // iterators may be similarly affected, so we've marked input for now.
    // Also note, in the vector<path>(p.begin(), p.end()) case, the user actually probably wants
    // input behavior, as distance()-ing a path is fairly expensive.
    using iterator_category = std::input_iterator_tag;
    using value_type        = path;
    using difference_type   = ptrdiff_t;
    using pointer           = const path*;
    using reference         = const path&;

    _Path_iterator() = default;

    _Path_iterator(const _Base_iter& _Position_, const path* _Mypath_) noexcept
        : _Position(_Position_), _Element(), _Mypath(_Mypath_) {}

    _Path_iterator(const _Base_iter& _Position_, std::string_view _Element_text, const path* _Mypath_)
        : _Position(_Position_), _Element(_Element_text), _Mypath(_Mypath_) {}

    _Path_iterator(const _Path_iterator&) = default;
    _Path_iterator(_Path_iterator&&)      = default;
    _Path_iterator& operator=(const _Path_iterator&) = default;
    _Path_iterator& operator=(_Path_iterator&&) = default;

    reference operator*() const noexcept {
        return _Element;
    }

    pointer operator->() const noexcept {
        return std::addressof(_Element);
    }

    _Path_iterator& operator++() {
        const auto& _Text = _Mypath->native();
        const auto _Size  = _Element.native().size();
        //_Verify_range(_Text.begin(), _Position); // engaged when *this is checked
        const char* _Begin = _Text.data();
        const char* _End   = _Begin + _Text.size();
        //_Verify_range(_Begin, _Position); // engaged when *this is unchecked
        if (_Begin == __Get_unwrapped(_Position)) { // test if the next element will be root-directory
            _Position += static_cast<ptrdiff_t>(_Size);
            const auto _First              = _Text.data();
            const auto _Last               = _First + _Text.size();
            const auto _Root_name_end      = _Find_root_name_end(_First, _Last);
            const auto _Root_directory_end = std::find_if_not(_Root_name_end, _Last, _Is_slash);
            if (_First != _Root_name_end && _Root_name_end != _Root_directory_end) {
                // current element is root-name, root-directory exists, so next is root-directory
                _Element._Text.assign(_Root_name_end, _Root_directory_end);
                return *this;
            }

            // If we get here, either there is no root-name, and by !_Is_slash(*_Position), no root-directory,
            // or, current element is root-name, and root-directory doesn't exist.
            // Either way, the next element is the first of relative-path
        } else if (_Is_slash(*_Position)) { // current element is root-directory, or the "magic empty path"
            if (_Size == 0) { // current element was "magic empty path", become end()
                ++_Position;
                return *this;
            }

            // current element was root-directory, advance to relative-path
            _Position += static_cast<ptrdiff_t>(_Size);
        } else { // current element is one of relative-path
            _Position += static_cast<ptrdiff_t>(_Size);
        }

        if (__Get_unwrapped(_Position) == _End) {
            _Element.clear();
            return *this;
        }

        // at this point, the next element is a standard filename from relative_path(), and _Position
        // points at the preferred-separator or fallback-separator after the previous element
        while (_Is_slash(*_Position)) { // advance to the start of the following path element
            if (__Get_unwrapped(++_Position) == _End) { // "magic" empty element selected
                --_Position;
                _Element.clear();
                return *this;
            }
        }

        _Element._Text.assign(__Get_unwrapped(_Position), std::find_if(__Get_unwrapped(_Position), _End, _Is_slash));
        return *this;
    }

    _Path_iterator operator++(int) {
        _Path_iterator _Tmp = *this;
        ++*this;
        return _Tmp;
    }

    _Path_iterator& operator--() {
        const auto& _Text = _Mypath->native();
        //_Verify_range(_Text.begin(), _Position); // engaged when *this is checked
        const auto _First = _Text.data();
        //_Verify_range(_First, _Position); // engaged when *this is unchecked
        const auto _Last                   = _First + _Text.size();
        const auto _Root_name_end_ptr      = _Find_root_name_end(_First, _Last);
        const auto _Root_directory_end_ptr = std::find_if_not(_Root_name_end_ptr, _Last, _Is_slash);

        if (_Root_name_end_ptr != _Root_directory_end_ptr && __Get_unwrapped(_Position) == _Root_directory_end_ptr) {
            // current element is the first of relative-path, and the prev element is root-directory
            __Seek_wrapped(_Position, _Root_name_end_ptr);
            _Element._Text.assign(
                _Root_name_end_ptr, static_cast<size_t>(_Root_directory_end_ptr - _Root_name_end_ptr));
            return *this;
        }

        if (_First != _Root_name_end_ptr && __Get_unwrapped(_Position) == _Root_name_end_ptr) {
            // current element is root-directory or, if that doesn't exist, first
            // element of relative-path prev element therefore is root-name
            __Seek_wrapped(_Position, _First);
            _Element._Text.assign(_First, static_cast<size_t>(_Root_name_end_ptr - _First));
            return *this;
        }

        // from here, the result will be somewhere in relative-path
        if (__Get_unwrapped(_Position) == _Last && _Is_slash(_Position[-1])) { // target is "magic empty path"
            --_Position;
            _Element.clear();
            return *this;
        }

        while (_Root_directory_end_ptr != __Get_unwrapped(_Position) && _Is_slash(_Position[-1])) {
            --_Position;
        }

        const auto _New_end = _Position;
        while (_Root_directory_end_ptr != __Get_unwrapped(_Position) && !_Is_slash(_Position[-1])) {
            --_Position;
        }

        _Element._Text.assign(_Position, _New_end);
        return *this;
    }

    _Path_iterator operator--(int) {
        _Path_iterator _Tmp = *this;
        --*this;
        return _Tmp;
    }

    friend bool operator==(const _Path_iterator& _Lhs, const _Path_iterator& _Rhs) {
        return _Lhs._Position == _Rhs._Position;
    }

    friend bool operator!=(const _Path_iterator& _Lhs, const _Path_iterator& _Rhs) {
        return _Lhs._Position != _Rhs._Position;
    }

#if _ITERATOR_DEBUG_LEVEL != 0
    //fried _Verify_range(const _Path_iterator& _Lhs, const _Path_iterator& _Rhs) {
    //    _Verify_range(_Lhs._Position, _Rhs._Position);
    //}
#endif // _ITERATOR_DEBUG_LEVEL != 0

    using _Prevent_inheriting_unwrap = _Path_iterator;

    template <class _Iter2 = _Base_iter, std::enable_if_t<_Unwrappable_v<const _Iter2&>, int> = 0>
    _Path_iterator<_Unwrapped_t<const _Iter2&>> _Unwrapped() const {
        auto unwrapped = _Mypath->_Text.data() + (_Position - _Mypath->_Text.cbegin());
        return { unwrapped, _Element.native(), _Mypath };
    }

    static constexpr bool _Unwrap_when_unverified = _Do_unwrap_when_unverified_v<_Base_iter>;

    template <class _Other>
    friend class _Path_iterator;

    template <class _Other, std::enable_if_t<__Wrapped_seekable_v<_Base_iter, const _Other&>, int> = 0>
    constexpr void _Seek_to(const _Path_iterator<_Other>& _It) {
        _Position._Seek_to(_It._Position);
        _Element = _It._Element;
        // _Mypath intentionally unchanged
    }

private:
    // if the current element is the "magic empty path",
    //   points to preferred-separator or fallback-separator immediately before
    // otherwise, points to the first element of _Element
    _Base_iter _Position{};
    path _Element{};
    const path* _Mypath{};
};

inline void swap(path& _Left, path& _Right) noexcept {
    _Left.swap(_Right);
}

inline size_t hash_value(const path& _Path) noexcept {
    return std::hash<std::string>()(_Path.string());
}

inline path path::lexically_relative(const path& _Base) const {
    using namespace std::string_view_literals; // TRANSITION, VSO-571749

    constexpr std::string_view _Dot     = "."sv;
    constexpr std::string_view _Dot_dot = ".."sv;

    path _Result;

    if (root_name() != _Base.root_name() || is_absolute() != _Base.is_absolute()
        || (!has_root_directory() && _Base.has_root_directory())) {
        return _Result;
    }

    const iterator _This_end   = end();
    const iterator _Base_begin = _Base.begin();
    const iterator _Base_end   = _Base.end();

    auto _Mismatched  = std::mismatch(begin(), _This_end, _Base_begin, _Base_end);
    iterator& _A_iter = _Mismatched.first;
    iterator& _B_iter = _Mismatched.second;

    if (_A_iter == _This_end && _B_iter == _Base_end) {
        _Result = _Dot;
        return _Result;
    }

    { // Skip root-name and root-directory elements, N4810 29.11.7.5 [fs.path.itr]/4.1, 4.2
        ptrdiff_t _B_dist = std::distance(_Base_begin, _B_iter);

        const ptrdiff_t _Base_root_dist =
            static_cast<ptrdiff_t>(_Base.has_root_name()) + static_cast<ptrdiff_t>(_Base.has_root_directory());

        while (_B_dist < _Base_root_dist) {
            ++_B_iter;
            ++_B_dist;
        }
    }

    ptrdiff_t _Num = 0;

    for (; _B_iter != _Base_end; ++_B_iter) {
        const path& _Elem = *_B_iter;

        if (_Elem.empty()) { // skip empty element, N4810 29.11.7.5 [fs.path.itr]/4.4
        } else if (_Elem == _Dot) { // skip filename elements that are dot, N4810 29.11.7.4.11 [fs.path.gen]/4.2
        } else if (_Elem == _Dot_dot) {
            --_Num;
        } else {
            ++_Num;
        }
    }

    if (_Num < 0) {
        return _Result;
    }

    if (_Num == 0 && (_A_iter == _This_end || _A_iter->empty())) {
        _Result = _Dot;
        return _Result;
    }

    for (; _Num > 0; --_Num) {
        _Result /= _Dot_dot;
    }

    for (; _A_iter != _This_end; ++_A_iter) {
        _Result /= *_A_iter;
    }

    return _Result;
}

inline path::iterator path::begin() const {
    const auto _First         = _Text.data();
    const auto _Last          = _First + _Text.size();
    const auto _Root_name_end = _Find_root_name_end(_First, _Last);
    const char* _First_end;
    if (_First == _Root_name_end) { // first element isn't root-name
        auto _Root_directory_end = std::find_if_not(_Root_name_end, _Last, _Is_slash);
        if (_First == _Root_directory_end) { // first element is first relative-path entry
            _First_end = std::find_if(_Root_directory_end, _Last, _Is_slash);
        } else { // first element is root-directory
            _First_end = _Root_directory_end;
        }
    } else { // first element is root-name
        _First_end = _Root_name_end;
    }

    return iterator(_Text.cbegin(), std::string_view(_First, static_cast<size_t>(_First_end - _First)), this);
}

inline path::iterator path::end() const noexcept /* strengthened */ {
    return iterator(_Text.cend(), this);
}

} // system
} // mw

export namespace std
{
template<>
struct hash<mw::system::path>
{
    std::size_t operator()(const mw::system::path& p) const noexcept {
        return std::hash<std::string_view>()(p.c_str());
    }
};
}

#pragma pop_macro("new")
#pragma warning(pop)
#pragma pack(pop)
