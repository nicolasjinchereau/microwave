#pragma once
#include <variant>
#include <cstddef>
#include <cstdint>
#include <string>

namespace mw {
inline namespace utilities {

enum class PPTokenType
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

struct PPToken
{
    typedef std::variant<std::nullptr_t, int64_t, double, bool, char, std::string> DataStorageType;

    DataStorageType data;
    PPTokenType type = PPTokenType::EndOfFile;
    std::string::iterator pos;

    PPToken(){}

    PPToken(PPTokenType type, std::string::iterator pos, const std::string& value)
        : data(value), type(type), pos(pos){}

    PPToken(PPTokenType type, std::string::iterator pos, std::string&& value)
        : data(std::move(value)), type(type), pos(pos){}

    PPToken(PPTokenType type, std::string::iterator pos, int64_t value)
        : data(value), type(type), pos(pos){}

    PPToken(PPTokenType type, std::string::iterator pos, double value)
        : data(value), type(type), pos(pos){}

    PPToken(PPTokenType type, std::string::iterator pos, bool value)
        : data(value), type(type), pos(pos){}

    PPToken(PPTokenType type, std::string::iterator pos, char value)
        : data(value), type(type), pos(pos){}

    PPToken(PPTokenType type, std::string::iterator pos, std::nullptr_t value)
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

class Preprocessor
{
public:


};


} // utilities
} // mw
