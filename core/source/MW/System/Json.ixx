/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.System.Json;
import Microwave.System.Pointers;
import Microwave.System.Path;
import <algorithm>;
import <array>;
import <cassert>;
import <cfloat>;
import <charconv>;
import <cstdint>;
import <cstddef>;
import <forward_list>;
import <iostream>;
import <initializer_list>;
import <list>;
import <map>;
import <optional>;
import <stdexcept>;
import <string>;
import <sstream>;
import <type_traits>;
import <unordered_map>;
import <utf8.h>;
import <utility>;
import <vector>;
import <variant>;

export namespace mw {
inline namespace system {

enum class JsonTokenType
{
    EndOfFile,
    ObjectStart,
    ObjectEnd,
    ArrayStart,
    ArrayEnd,
    Colon,
    Comma,
    String,
    Integer,
    Float,
    Boolean,
    Null
};

struct JsonToken
{
    typedef std::variant<std::nullptr_t, int64_t, double, bool, char, std::string> DataStorageType;

    DataStorageType data;
    JsonTokenType type = JsonTokenType::EndOfFile;
    std::string::iterator pos;

    JsonToken(){}

    JsonToken(JsonTokenType type, std::string::iterator pos, const std::string& value)
        : data(value), type(type), pos(pos){}

    JsonToken(JsonTokenType type, std::string::iterator pos, std::string&& value)
        : data(std::move(value)), type(type), pos(pos){}

    JsonToken(JsonTokenType type, std::string::iterator pos, int64_t value)
        : data(value), type(type), pos(pos){}

    JsonToken(JsonTokenType type, std::string::iterator pos, double value)
        : data(value), type(type), pos(pos){}

    JsonToken(JsonTokenType type, std::string::iterator pos, bool value)
        : data(value), type(type), pos(pos){}

    JsonToken(JsonTokenType type, std::string::iterator pos, char value)
        : data(value), type(type), pos(pos){}

    JsonToken(JsonTokenType type, std::string::iterator pos, std::nullptr_t value)
        : data(value), type(type), pos(pos){}

    int64_t GetInteger() const {
        return std::get<int64_t>(data);
    }

    double GetFloat() const {
        return std::get<double>(data);
    }

    bool GetBoolean() const {
        return std::get<bool>(data);
    }

    char GetChar() const {
        return std::get<char>(data);
    }

    const std::string& GetString() const {
        return std::get<std::string>(data);
    }

    std::string_view GetTypeName() const
    {
        static std::unordered_map<JsonTokenType, std::string_view> typeNames {
            { JsonTokenType::EndOfFile, "EndOfFile" },
            { JsonTokenType::ObjectStart, "ObjectStart" },
            { JsonTokenType::ObjectEnd, "ObjectEnd" },
            { JsonTokenType::ArrayStart, "ArrayStart" },
            { JsonTokenType::ArrayEnd, "ArrayEnd" },
            { JsonTokenType::Colon, "Colon" },
            { JsonTokenType::Comma, "Comma" },
            { JsonTokenType::String, "String" },
            { JsonTokenType::Integer, "Integer" },
            { JsonTokenType::Float, "Float" },
            { JsonTokenType::Boolean, "Boolean" },
            { JsonTokenType::Null, "Null" }
        };

        return typeNames[type];
    }
};

class JsonLexer
{
    size_t line{};
    size_t column{};
    std::string::iterator pos;
    std::string::iterator next;
    char32_t value{};
    std::string chars;
    int tabLength = 4;
public:

    bool endOfFile() const {
        return pos == chars.end();
    }

    char32_t getValue() const {
        return value;
    }

    size_t getOffset() const {
        return pos - chars.begin();
    }

    JsonLexer(const std::string& text)
    {
        chars = text;

        pos = chars.begin();
        next = pos;
        value = utf8::next(next, chars.end());
        line = 0;
        column = 0;
    }

    static std::vector<JsonToken> Tokenize(const std::string& text)
    {
        std::vector<JsonToken> tokens;
        JsonLexer lexer(text);

        do {
            tokens.push_back(lexer.GetNextToken());
        } while (!lexer.endOfFile());

        return tokens;
    }

    JsonToken GetNextToken()
    {
        SkipWhitespace();

        if (pos == chars.end()) {
            return JsonToken(JsonTokenType::EndOfFile, pos, (char)EOF);
        }
        else if (value == '{') {
            SkipChar();
            return JsonToken(JsonTokenType::ObjectStart, pos, '{');
        }
        else if (value == '}') {
            SkipChar();
            return JsonToken(JsonTokenType::ObjectEnd, pos, '}');
        }
        else if (value == '[') {
            SkipChar();
            return JsonToken(JsonTokenType::ArrayStart, pos, '[');
        }
        else if (value == ']') {
            SkipChar();
            return JsonToken(JsonTokenType::ArrayEnd, pos, ']');
        }
        else if (value == ':') {
            SkipChar();
            return JsonToken(JsonTokenType::Colon, pos, ':');
        }
        else if (value == ',') {
            SkipChar();
            return JsonToken(JsonTokenType::Comma, pos, ',');
        }
        else if (value == '\"') {
            return GetStringToken();
        }
        else if ((value >= '0' && value <= '9') || value == '-' || value == '+') {
            return GetNumberToken();
        }
        else if (value == 't' || value == 'f') {
            return GetBooleanToken();
        }
        else if (value == 'n') {
            return GetNullToken();
        }
        else {
            throw std::runtime_error(std::string("found unexpected input: ") + (char)value);
        }
    }

private:

    void SkipWhitespace()
    {
        while (pos != chars.end())
        {
            if (value == ' ') // spaces
            {
                ++column;
            }
            else if (value == '\t')
            {
                column += tabLength;
            }
            else if (value == '\n') // new line
            {
                ++line;
                column = 0;
            }
            else if (value == '\v' || value == '\f' || value == '\r')
            {
                // ignore
            }
            else
            {
                // found non-whitespace character
                break;
            }

            SkipChar();
        }
    }

    void SkipChar()
    {
        assert(pos != next);
        pos = next;
        value = (next != chars.end()) ?
            utf8::next(next, chars.end()) : 0;
    }

    void SkipChars(ptrdiff_t count)
    {
        assert(pos != next);
        utf8::advance(pos, count, chars.end());
        next = pos;
        value = (next != chars.end()) ?
            utf8::next(next, chars.end()) : 0;
    }

    bool IsStartOfNumber(char32_t c) {
        return (c >= '0' && c <= '9') || c == '-' || c == '+';
    }

    bool IsStartOf(char32_t c) {
        return (c >= '0' && c <= '9') || c == '-' || c == '+';
    }

    JsonToken GetStringToken()
    {
        assert(value == U'\"');

        auto start = pos;
        SkipChar();

        std::string str;

        while (pos != chars.end())
        {
            if (value == U'\"')
            {
                SkipChar();
                return JsonToken(JsonTokenType::String, start, str);
            }
            else if (value == U'\\')
            {
                SkipChar();

                if (pos == chars.end())
                    throw std::runtime_error("unexpected end of input");

                if (value == U'\"') {
                    SkipChar();
                    utf8::append(U'\"', std::back_inserter(str));
                }
                else if (value == U'\\') {
                    SkipChar();
                    utf8::append(U'\\', std::back_inserter(str));
                }
                else if (value == U'r') {
                    SkipChar();
                    utf8::append(U'\r', std::back_inserter(str));
                }
                else if (value == U'n') {
                    SkipChar();
                    utf8::append(U'\n', std::back_inserter(str));
                }
                else if (value == U't') {
                    SkipChar();
                    utf8::append(U'\t', std::back_inserter(str));
                }
                else if (value == U'b') {
                    SkipChar();
                    utf8::append(U'\b', std::back_inserter(str));
                }
                else if (value == U'f') {
                    SkipChar();
                    utf8::append(U'\f', std::back_inserter(str));
                }
                else if (value == U'u')
                {
                    SkipChar();

                    if ((chars.end() - pos) < 4)
                        throw std::runtime_error("unexpected end of input");

                    char hex[4];

                    for (int i = 0; i < 4; ++i)
                    {
                        if (!isxdigit(value))
                            throw std::runtime_error("invalid unicode escape sequence");

                        hex[i] = (char)value;
                        SkipChar();
                    }

                    char32_t charValue = (char32_t)std::strtoul(hex, nullptr, 16);
                    utf8::append(charValue, std::back_inserter(str));
                }
                else
                {
                    utf8::append(value, std::back_inserter(str));
                    SkipChar();
                }
            }
            else
            {
                utf8::append(value, std::back_inserter(str));
                SkipChar();
            }
        }

        assert(pos == chars.end());
        throw std::runtime_error("unexpected end of input");
    }

    JsonToken GetNumberToken()
    {
        auto start = pos;
        auto beg = chars.data() + (pos - chars.begin());
        auto end = chars.data() + chars.size();

        double value;
        std::from_chars_result ret = std::from_chars(beg, end, value);
        if(ret.ec != std::errc())
            throw std::runtime_error("invalid number");

        auto len = ret.ptr - &start[0];
        std::string_view number(beg, beg + len);

        SkipChars(len);

        if(number.find('.') != std::string_view::npos)
        {
            return JsonToken(JsonTokenType::Float, start, value);
        }
        else
        {
            int64_t intValue;
            ret = std::from_chars(beg, end, intValue);
            if(ret.ec != std::errc())
                throw std::runtime_error("invalid number");

            return JsonToken(JsonTokenType::Integer, start, intValue);
        }
    }

    JsonToken GetBooleanToken()
    {
        auto start = pos;
        auto p = &*pos;

        bool val;

        if (std::equal(p, p + 4, "true")) {
            val = true;
            SkipChars(4);
        }
        else if (std::equal(p, p + 5, "false")) {
            val = false;
            SkipChars(5);
        }
        else {
            throw std::runtime_error("expected boolean literal");
        }

        return JsonToken(JsonTokenType::Boolean, start, val);
    }

    JsonToken GetNullToken()
    {
        auto start = pos;
        auto p = &*pos;

        if (std::equal(p, p + 4, "null"))
            SkipChars(4);
        else
            throw std::runtime_error("expected null literal");

        return JsonToken(JsonTokenType::Null, start, nullptr);
    }
};

class json;

class JsonParser
{
    JsonLexer lexer;
    JsonToken token;
public:
    JsonParser(const std::string& text);
    json parse();

private:
    bool NextToken(bool thrownOnEOF = true);
    json ParseValue();
    json ParseString();
    json ParseInteger();
    json ParseFloat();
    json ParseBoolean();
    json ParseNull();
    json ParseObject();
    json ParseArray();
};

class JsonPrinter
{
    int indentWidth;
    bool pretty;

    void Indent(std::ostream& stream, int indent);
    void WriteEscaped(std::ostream& stream, const std::string& val);
public:

    JsonPrinter(int indentWidth);

    std::string ToString(const json& val);
    void ToStream(std::ostream& stream, int indent, const json& val);
};

template <typename T>
class has_to_string
{
    template <typename C, typename = decltype(to_string(std::declval<std::string&>(), std::declval<const T>()))>
    static std::true_type test(int);

    template <typename C>
    static std::false_type test(...);

public:
    static constexpr bool value = decltype(test<T>(0))::value;
};

template <typename T>
class has_from_string
{
    template <typename C, typename = decltype(from_string(std::declval<const std::string>(), std::declval<T&>()))>
    static std::true_type test(int);

    template <typename C>
    static std::false_type test(...);

public:
    static constexpr bool value = decltype(test<T>(0))::value;
};

template<class T>
inline constexpr bool has_from_string_v = has_from_string<T>::value;

template<class T>
inline constexpr bool has_to_string_v = has_to_string<T>::value;

template<class>
inline constexpr bool is_initializer_list = false;

template<class T>
inline constexpr bool is_initializer_list<std::initializer_list<T>> = true;

enum class DataType
{
    Null,
    Object,
    Array,
    String,
    Integer,
    Float,
    Boolean
};

class json
{
public:
    using NullType = std::nullptr_t;
    using ObjectType = std::unordered_map<std::string, json>;
    using ArrayType = std::vector<json>;
    using StringType = std::string;
    using IntegerType = int64_t;
    using FloatType = double;
    using BooleanType = bool;

    friend JsonParser;

private:
    typedef std::variant<NullType, ObjectType, ArrayType, StringType, IntegerType, FloatType, BooleanType> DataStorageType;
    DataStorageType data;

public:
    json(nullptr_t = nullptr)
        : data(nullptr) {}

    json(const ObjectType& obj)
        : data(obj) {}

    json(ObjectType&& obj)
        : data(std::move(obj)) {}

    json(const ArrayType& arr)
        : data(arr) {}

    json(ArrayType&& arr)
        : data(std::move(arr)) {}

    json(const StringType& str)
        : data(str) {}

    json(StringType&& str)
        : data(std::move(str)) {}

    template<class T> requires (std::is_integral_v<T> && !std::is_same_v<T, bool>)
    json(const T& integer)
        : data((IntegerType)integer) {}

    template<class T> requires std::is_floating_point_v<T>
    json(T floating)
        : data((FloatType)floating) {}

    template<class T> requires std::is_same_v<T, BooleanType>
    json(T boolean)
        : data(boolean) {}

    template<class T, class S = std::decay_t<T>> requires (
        !std::is_same_v<S, json> &&
        !std::is_integral_v<S> &&
        !std::is_same_v<S, BooleanType> &&
        !std::is_floating_point_v<S>)
    json(T&& val) : json() {
        to_json(*this, std::forward<T>(val));
    }

    static json object() {
        json ret;
        ret.data.emplace<ObjectType>();
        return ret;
    }

    static json array() {
        json ret;
        ret.data.emplace<ArrayType>();
        return ret;
    }

    static json string() {
        json ret;
        ret.data.emplace<StringType>();
        return ret;
    }

    template<class T> requires (!std::is_same_v<std::decay_t<T>, json>)
    json& operator=(T&& val) {
        to_json(*this, std::forward<T>(val));
        return *this;
    }

    using StringValueType = StringType::value_type;

    template<class T> requires (
        !std::is_same_v<T, json>
        && !std::is_void_v<T>
        && !std::is_pointer_v<T>
        && !std::is_same_v<T, std::nullptr_t>
        && !std::is_same_v<T, StringValueType>
        && !is_initializer_list<T>
        && !std::is_same_v<T, std::string_view>
        )
    operator T() const {
        return get<T>();
    }

    static json parse(const std::string& text) {
        JsonParser parser(text);
        return parser.parse();
    }

    std::string dump(int indent = -1)
    {
        JsonPrinter printer(indent);
        return printer.ToString(*this);
    }

    DataType GetType() const
    {
        std::array<DataType, 7> types {
            DataType::Null,
            DataType::Object,
            DataType::Array,
            DataType::String,
            DataType::Integer,
            DataType::Float,
            DataType::Boolean
        };
        
        return types[data.index()];
    }

    bool IsNull() const {
        return std::holds_alternative<NullType>(data);
    }

    bool IsObject() const {
        return std::holds_alternative<ObjectType>(data);
    }

    bool IsArray() const {
        return std::holds_alternative<ArrayType>(data);
    }

    bool IsString() const {
        return std::holds_alternative<StringType>(data);
    }

    bool IsInteger() const {
        return std::holds_alternative<IntegerType>(data);
    }

    bool IsFloat() const {
        return std::holds_alternative<FloatType>(data);
    }

    bool IsBoolean() const {
        return std::holds_alternative<BooleanType>(data);
    }

    bool is_null() const {
        return IsNull();
    }

    json& at(size_t index) {
        return GetArray().at(index);
    }

    const json& at(size_t index) const {
        return GetArray().at(index);
    }

    json& at(const ObjectType::key_type& key) {
        return GetObject().at(key);
    }

    const json& at(const ObjectType::key_type& key) const {
        return GetObject().at(key);
    }
    
    json& operator[](size_t index)
    {
        if (IsNull())
            data.emplace<ArrayType>();

        if (index >= GetArray().size())
            GetArray().resize(index + 1);

        return GetArray()[index];
    }

    const json& operator[](size_t index) const {
        return GetArray().at(index);
    }

    json& operator[](const ObjectType::key_type& key)
    {
        if (IsNull())
            data.emplace<ObjectType>();

        return GetObject()[key];
    }

    json& operator[](const char* key)
    {
        if (IsNull())
            data.emplace<ObjectType>();

        return GetObject()[key];
    }

    const json& operator[](const ObjectType::key_type& key) const {
        return GetObject().at(key);
    }

    const json& operator[](const char* key) const {
        return GetObject().at(key);
    }
    
    ObjectType& GetObject() {
        return std::get<ObjectType>(data);
    }

    const ObjectType& GetObject() const {
        return std::get<ObjectType>(data);
    }

    ArrayType& GetArray() {
        return std::get<ArrayType>(data);
    }

    const ArrayType& GetArray() const {
        return std::get<ArrayType>(data);
    }

    StringType& GetString() {
        return std::get<StringType>(data);
    }

    const StringType& GetString() const {
        return std::get<StringType>(data);
    }

    IntegerType& GetInteger() {
        return std::get<IntegerType>(data);
    }

    const IntegerType& GetInteger() const {
        return std::get<IntegerType>(data);
    }

    FloatType& GetFloat() {
        return std::get<FloatType>(data);
    }

    const FloatType& GetFloat() const {
        return std::get<FloatType>(data);
    }

    BooleanType& GetBoolean() {
        return std::get<BooleanType>(data);
    }

    const BooleanType& GetBoolean() const {
        return std::get<BooleanType>(data);
    }

    template<class T>
    T get() const
    {
        T val;
        from_json(*this, val);
        return val;
    }

    template<class T>
    T get(T defaultValue) const
    {
        T val;
        
        if (!IsNull())
            from_json(*this, val);
        else
            val = defaultValue;

        return val;
    }

    template<>
    json get<json>() const {
        return *this;
    }

    template<class T>
    T value(const ObjectType::key_type& key, T defaultValue) const
    {
        T ret;

        auto it = GetObject().find(key);
        if (it != GetObject().end()) {
            ret = it->second.get<T>();
        }
        else {
            ret = std::move(defaultValue);
        }

        return ret;
    }

    bool empty() const
    {
        if (IsObject())
            return GetObject().empty();
        else if (IsArray())
            return GetArray().empty();
        else if (IsString())
            return GetString().empty();
        else if (IsNull())
            return true;
        else // IsInteger() || IsFloat() || IsBoolean()
            return false;
    }

    size_t size() const
    {
        if (IsObject())
            return GetObject().size();
        else if (IsArray())
            return GetArray().size();
        else if (IsString())
            return GetString().size();
        else if (IsNull())
            return 0;
        else // IsInteger() || IsFloat() || IsBoolean()
            return 1;
    }

    void clear()
    {
        if (IsObject())
            GetObject().clear();
        else if (IsArray())
            GetArray().clear();
        else if (IsString())
            GetString().clear();
        else if (IsInteger())
            GetInteger() = 0;
        else if (IsFloat())
            GetFloat() = 0;
        else if (IsBoolean())
            GetBoolean() = false;
    }
    
    void push_back(const json& val)
    {
        if (IsNull())
            data.emplace<ArrayType>();

        GetArray().push_back(val);
    }

    void push_back(json&& val)
    {
        if (IsNull())
            data.emplace<ArrayType>();

        GetArray().push_back(std::move(val));
    }
    
    template<class ObjectIter, class ArrayIter, class NullIter = std::nullptr_t>
    class base_const_iterator
    {
        std::variant<NullIter, ObjectIter, ArrayIter> var;
    public:
        base_const_iterator() : var(NullIter{}){}
        base_const_iterator(ObjectIter it) : var(it){}
        base_const_iterator(ArrayIter it) : var(it){}

        bool IsNullIter() const {
            return std::holds_alternative<NullIter>(var);
        }

        bool IsObjectIter() const {
            return std::holds_alternative<ObjectIter>(var);
        }

        bool IsArrayIter() const {
            return std::holds_alternative<ArrayIter>(var);
        }

        NullIter GetNullIter() const {
            return std::get<NullIter>(var);
        }

        ObjectIter GetObjectIter() const {
            return std::get<ObjectIter>(var);
        }

        ObjectIter& GetObjectIter() {
            return std::get<ObjectIter>(var);
        }

        ArrayIter GetArrayIter() const {
            return std::get<ArrayIter>(var);
        }

        ArrayIter& GetArrayIter() {
            return std::get<ArrayIter>(var);
        }

        const ObjectType::key_type& key() const {
            return GetObjectIter()->first;
        }

        const json& value() const
        {
            if (IsObjectIter())
                return GetObjectIter()->second;
            else if (IsArrayIter())
                return *GetArrayIter();

            throw std::runtime_error("null");
        }

        const json& operator*() const {
            return value();
        }

        const json* operator->() const {
            return &value();
        }

        base_const_iterator operator++(int)
        {
            base_const_iterator ret = *this;

            if (IsObjectIter())
                ++GetObjectIter();
            else if (IsArrayIter())
                ++GetArrayIter();
            else
                throw std::runtime_error("null");

            return ret;
        }

        base_const_iterator& operator++()
        {
            if (IsObjectIter())
                ++GetObjectIter();
            else if (IsArrayIter())
                ++GetArrayIter();
            else
                throw std::runtime_error("null");

            return *this;
        }

        bool operator==(const base_const_iterator& it) const
        {
            assert(var.index() == it.var.index());

            bool result;

            if (IsObjectIter() && it.IsObjectIter())
                result = GetObjectIter() == it.GetObjectIter();
            else if (IsArrayIter() && it.IsArrayIter())
                result = GetArrayIter() == it.GetArrayIter();
            else
                result = GetNullIter() == it.GetNullIter();

            return result;
        }

        bool operator!=(const base_const_iterator& it) const {
            return !(*this == it);
        }
    };

    template<class ObjectIter, class ArrayIter>
    class base_iterator : public base_const_iterator<ObjectIter, ArrayIter>
    {
    public:
        json& value()
        {
            if (this->IsObjectIter())
                return this->GetObjectIter()->second;
            else if (this->IsArrayIter())
                return *this->GetArrayIter();

            throw std::runtime_error("null");
        }

        json& operator*() {
            return value();
        }

        json* operator->() {
            return &value();
        }
    };

    using const_iterator = base_const_iterator<ObjectType::const_iterator, ArrayType::const_iterator>;
    using iterator = base_iterator<ObjectType::iterator, ArrayType::iterator>;

    iterator begin()
    {
        if(IsObject())
            return { GetObject().begin() };
        else if(IsArray())
            return { GetArray().begin() };

        return {};
    }

    iterator end()
    {
        if(IsObject())
            return { GetObject().end() };
        else if(IsArray())
            return { GetArray().end() };

        return {};
    }

    const_iterator begin() const
    {
        if(IsObject())
            return { GetObject().cbegin() };
        else if(IsArray())
            return { GetArray().cbegin() };

        return {};
    }

    const_iterator end() const
    {
        if(IsObject())
            return { GetObject().cend() };
        else if(IsArray())
            return { GetArray().cend() };

        return {};
    }

    const_iterator find(const ObjectType::key_type& key) const
    {
        if (IsObject())
            return { GetObject().find(key) };
        
        return {};
    }

    iterator find(const ObjectType::key_type& key)
    {
        if (IsObject())
            return { GetObject().find(key) };

        return {};
    }

    const ObjectType& items() const {
        return GetObject();
    }

    ObjectType& items() {
        return GetObject();
    }
};

void to_json(json& obj, const json::ObjectType& val) {
    obj = json(val);
}

void from_json(const json& obj, json::ObjectType& val) {
    val = obj.GetObject();
}

void to_json(json& obj, const json::ArrayType& val) {
    obj = json(val);
}

void from_json(const json& obj, json::ArrayType& val) {
    val = obj.GetArray();
}

template<class T> requires std::is_constructible_v<typename json::StringType, T>
void to_json(json& obj, const T& val) {
    obj = json(json::StringType(val));
}

void from_json(const json& obj, json::StringType& val) {
    val = obj.GetString();
}

template<class T> requires (std::is_integral_v<T> && !std::is_same_v<T, bool>)
void to_json(json& obj, const T& val) {
    obj = json((json::IntegerType)val);
}

template<class T> requires (std::is_integral_v<T> && !std::is_same_v<T, bool>)
void from_json(const json& obj, T& val) {
    val = (T)obj.GetInteger();
}

template<class T> requires std::is_floating_point_v<T>
void to_json(json& obj, const T& val) {
    obj = json((json::FloatType)val);
}

template<class T> requires std::is_floating_point_v<T>
void from_json(const json& obj, T& val) {
    val = (T)obj.GetFloat();
}

template<class T> requires std::is_same_v<typename json::BooleanType, T>
void to_json(json& obj, const T& val) {
    obj = json(val);
}

template<class T> requires std::is_same_v<typename json::BooleanType, T>
void from_json(const json& obj, T& val) {
    val = obj.GetBoolean();
}

void to_json(json& obj, const std::nullptr_t& val) {
    obj = json();
}

void from_json(const json& obj, std::nullptr_t& val) {
    val = nullptr;
}

template<class T>
void to_json(json& obj, const std::forward_list<T>& cont)
{
    json val = json::array();

    for(auto& elem : cont) {
        val.push_back(elem);
    }

    obj = std::move(val);
}

template<class T>
void from_json(const json& obj, std::forward_list<T>& cont)
{
    auto& arr = obj.GetArray();
    cont.clear();

    for (auto it = arr.rbegin(); it != arr.rend(); ++it) {
        cont.push_front(it->get<T>());
    }
}

template<class T>
void to_json(json& obj, const std::list<T>& cont)
{
    json val = json::array();

    for(auto& elem : cont) {
        val.push_back(elem);
    }

    obj = std::move(val);
}

template<class T>
void from_json(const json& obj, std::list<T>& cont)
{
    auto& arr = obj.GetArray();
    cont.clear();

    for(auto& val : arr) {
        cont.push_back(val.get<T>());
    }
}

template<class T, size_t N>
void to_json(json& obj, const std::array<T, N>& cont)
{
    json val = json::array();

    for(size_t i = 0; i != N; ++i) {
        val.push_back(cont[i]);
    }

    obj = std::move(val);
}

template<class T, size_t N>
void from_json(const json& obj, std::array<T, N>& cont)
{
    auto& arr = obj.GetArray();
    
    for(size_t i = 0; i != N; ++i) {
        cont[i] = arr[i].get<T>();
    }
}

template<class T>
void to_json(json& obj, const std::vector<T>& cont)
{
    json val = json::array();

    for(auto& elem : cont)
        val.push_back(elem);

    obj = std::move(val);
}

template<class T>
void from_json(const json& obj, std::vector<T>& cont)
{
    auto& arr = obj.GetArray();
    cont.clear();

    for(auto& val : arr) {
        cont.push_back(val.get<T>());
    }
}

template<class K, class T, class H> requires (std::is_constructible_v<typename json::StringType, K> || has_to_string<K>::value)
void to_json(json& obj, const std::unordered_map<K, T, H>& cont)
{
    json ret = json::object();
    auto& objectValue = ret.GetObject();

    for(auto& [key, value] : cont)
    {
        if constexpr (std::is_constructible_v<typename json::StringType, K>) {
            objectValue[key] = json(value);
        }
        else {
            std::string k;
            to_string(k, key);
            objectValue[k] = json(value);
        }
    }

    obj = std::move(ret);
}

template<class K, class T, class H> requires (std::is_constructible_v<K, typename json::StringType> || has_from_string<K>::value)
void from_json(const json& obj, std::unordered_map<K, T, H>& cont)
{
    auto& objectValue = obj.GetObject();
    cont.clear();

    for(auto& [key, val] : objectValue)
    {
        if constexpr (std::is_constructible_v<K, typename json::StringType>) {
            cont[key] = val.get<T>();
        }
        else {
            K k;
            from_string(key, k);
            cont[k] = val.get<T>();
        }
    }
}

template<class K, class T, class H>
void to_json(json& obj, const std::map<K, T, H>& cont)
{
    json ret = json::object();
    auto& objectValue = ret.GetObject();

    for(auto& [key, value] : cont)
        objectValue[json(key)] = json(value);

    obj = std::move(ret);
}

template<class K, class T, class H>
void from_json(const json& obj, std::map<K, T, H>& cont)
{
    auto& objectValue = obj.GetObject();
    cont.clear();

    for(auto& [key, val] : objectValue) {
        cont[json(key)] = val.get<T>();
    }
}

// json PARSER
JsonParser::JsonParser(const std::string& text)
    : lexer(text){}

json JsonParser::parse()
{
    if (!NextToken())
        throw std::runtime_error("input is empty");

    return ParseValue();
}

bool JsonParser::NextToken(bool thrownOnEOF) {
    token = std::move(lexer.GetNextToken());
    return token.type != JsonTokenType::EndOfFile;
}

json JsonParser::ParseValue()
{
    switch (token.type)
    {
    case JsonTokenType::ObjectStart:
        return ParseObject();
    case JsonTokenType::ArrayStart:
        return ParseArray();
    case JsonTokenType::String:
        return ParseString();
    case JsonTokenType::Integer:
        return ParseInteger();
    case JsonTokenType::Float:
        return ParseFloat();
    case JsonTokenType::Boolean:
        return ParseBoolean();
    case JsonTokenType::Null:
        return ParseNull();
    case JsonTokenType::EndOfFile:
        throw std::runtime_error("unexpected end of input");
    default:
        throw std::runtime_error("unexpected token");
    }
}

json JsonParser::ParseString()
{
    assert(token.type == JsonTokenType::String);
    auto ret = json(token.GetString());
    NextToken(false);
    return ret;
}

json JsonParser::ParseInteger()
{
    assert(token.type == JsonTokenType::Integer);
    auto ret = json(token.GetInteger());
    NextToken(false);
    return ret;
}

json JsonParser::ParseFloat()
{
    assert(token.type == JsonTokenType::Float);
    auto ret = json(token.GetFloat());
    NextToken(false);
    return ret;
}

json JsonParser::ParseBoolean()
{
    assert(token.type == JsonTokenType::Boolean);
    auto ret = json(token.GetBoolean());
    NextToken(false);
    return ret;
}

json JsonParser::ParseNull()
{
    assert(token.type == JsonTokenType::Null);
    auto ret = json();
    NextToken(false);
    return ret;
}

json JsonParser::ParseObject()
{
    assert(token.type == JsonTokenType::ObjectStart);
    NextToken();

    json::ObjectType values;

    while (token.type != JsonTokenType::ObjectEnd)
    {
        if (token.type != JsonTokenType::String)
            throw std::runtime_error("expected string");

        json key = ParseString();

        if (token.type != JsonTokenType::Colon)
            throw std::runtime_error("expected colon");

        NextToken();

        values[key.GetString()] = ParseValue();

        if (token.type == JsonTokenType::Comma)
        {
            NextToken();

            if (token.type == JsonTokenType::ObjectEnd)
                throw std::runtime_error("expected a value");
        }
        else
        {
            if (token.type != JsonTokenType::ObjectEnd)
                throw std::runtime_error("expected '}'");
        }
    }

    NextToken();

    return json(std::move(values));
}

json JsonParser::ParseArray()
{
    assert(token.type == JsonTokenType::ArrayStart);
    NextToken();

    std::vector<json> values;

    while (token.type != JsonTokenType::ArrayEnd)
    {
        values.push_back(ParseValue());

        if (token.type == JsonTokenType::Comma)
        {
            NextToken();

            if (token.type == JsonTokenType::ArrayEnd)
                throw std::runtime_error("expected a value");
        }
        else
        {
            if (token.type != JsonTokenType::ArrayEnd)
                throw std::runtime_error("expected ']'");
        }
    }

    NextToken();

    return json(std::move(values));
}

// json PRINTER
void JsonPrinter::Indent(std::ostream& stream, int indent)
{
    if(pretty)
    {
        int totalIndent = indent * indentWidth;

        for(int i = 0; i != totalIndent; ++i)
            stream.put(' ');
    }
}

JsonPrinter::JsonPrinter(int indentWidth)
    : indentWidth(indentWidth),
    pretty(indentWidth != -1)
{
}

std::string JsonPrinter::ToString(const json& value)
{
    std::stringstream stream;
    ToStream(stream, 0, value);
    return stream.str();
}

void JsonPrinter::WriteEscaped(std::ostream& stream, const std::string& val)
{
    stream << '\"';

    for(auto& c : val)
    {
        if(c == '\"')
            stream << '\\' << '\"';
        else if (c == '\\')
            stream << '\\' << '\\';
        else if(c == '\r')
            stream << '\\' << 'r';
        else if(c == '\n')
            stream << '\\' << 'n';
        else if(c == '\t')
            stream << '\\' << 't';
        else if(c == '\b')
            stream << '\\' << 'b';
        else if(c == '\f')
            stream << '\\' << 'f';
        else
            stream << c;
    }

    stream << '\"';
}

void JsonPrinter::ToStream(std::ostream& stream, int indent, const json& value)
{
    switch (value.GetType())
    {
    default:
    case DataType::Null:
        stream << "null";
        break;

    case DataType::Object:
    {
        auto& obj = value.GetObject();

        stream << '{';
        if (pretty && !obj.empty()) stream << '\n';

        int i = 0;
        for (auto& [key, val] : obj)
        {
            if (i++) {
                stream << ',';
                if (pretty) stream << '\n';
            }

            Indent(stream, indent + 1);

            WriteEscaped(stream, key);
            stream << ':';
            if (pretty) stream << ' ';
            ToStream(stream, indent + 1, val);
        }

        if (!obj.empty())
        {
            if (pretty) stream << '\n';
            Indent(stream, indent);
        }

        stream << '}';
        break;
    }
    case DataType::Array:
    {
        stream << '[';

        auto& arr = value.GetArray();

        if (pretty && !arr.empty()) stream << '\n';

        int i = 0;
        for (auto& elem : arr)
        {
            if (i++) {
                stream << ',';
                if (pretty) stream << '\n';
            }

            Indent(stream, indent + 1);
            ToStream(stream, indent + 1, elem);
        }

        if (!arr.empty())
        {
            if (pretty) stream << '\n';
            Indent(stream, indent);
        }

        stream << ']';
        break;
    }
    case DataType::String:
    {
        auto& val = value.GetString();
        WriteEscaped(stream, val);
        break;
    }
    case DataType::Integer:
    {
        auto integer = value.get<json::IntegerType>();
        stream << integer;
        break;
    }
    case DataType::Float:
    {
        auto floating = value.get<json::FloatType>();

        constexpr int MaxDigits = 325;
        char chars[MaxDigits];

        auto ret = std::to_chars(
            &chars[0], &chars[0] + MaxDigits,
            floating, std::chars_format::general);

        std::string_view str(&chars[0], ret.ptr);
        if(str.find('.') == std::string_view::npos)
        {
            auto p = ret.ptr;
            *p++ = '.';
            *p++ = '0';
            str = std::string_view(&chars[0], p);
        }

        stream << str;

        break;
    }
    case DataType::Boolean:
    {
        auto boolean = value.get<json::BooleanType>();
        stream << std::boolalpha << boolean;
        break;
    }
    }
}

template<class T>
void to_json(json& val, const sptr<T>& object) {
    val = object ? json(*object) : json(nullptr);
}

template<class T>
void from_json(const json& obj, sptr<T>& object)
{
    if(!obj.IsNull()) {
        object = spnew<T>();
        from_json(obj, *object);
    }
    else {
        object = nullptr;
    }
}

template<class T>
void to_json(json& obj, const std::optional<T>& opt) {
    obj = opt ? json(*opt) : json(nullptr);
}

template<class T>
void from_json(const json& obj, std::optional<T>& opt) {
    opt = !obj.IsNull() ? std::optional<T>(obj.get<T>()) : std::optional<T>();
}

void to_json(json& obj, const path& p) {
    obj = p.generic_string();
}

void from_json(const json& obj, path& p) {
    p = obj.get<std::string>();
}

} // system
} // mw
