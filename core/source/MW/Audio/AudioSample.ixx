/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Audio.AudioSample;
import <cassert>;
import <cstddef>;
import <cstdint>;
import <span>;
import <stdexcept>;
import <type_traits>;
import <algorithm>;

export namespace mw {
inline namespace audio {

enum class SampleType
{
    Unspecified,
    Int8,
    Int16,
    Int24,
    Int32,
    Int64,
    Float32,
    Float64
};

int GetBytesPerSample(SampleType type)
{
    switch (type)
    {
    default:
    case SampleType::Unspecified: return -1;
    case SampleType::Int8: return 1;
    case SampleType::Int16: return 2;
    case SampleType::Int24: return 3;
    case SampleType::Int32: return 4;
    case SampleType::Int64: return 8;
    case SampleType::Float32: return 4;
    case SampleType::Float64: return 8;
    };
}

template<auto IntMax, typename IntType>
    requires std::is_integral_v<IntType> && std::is_signed_v<IntType>
inline IntType FloatToInt(double value)
{
    value = value < -1 ? -1 : (value > 1 ? 1 : value);
    auto m = IntMax;
    if (value < 0) m += 1;
    return (IntType)(value * m);
}

template<auto IntMax, typename FloatType> requires std::is_floating_point_v<FloatType>
inline FloatType IntToFloat(int64_t value)
{
    auto m = IntMax;
    if (value < 0) m += 1;
    return (FloatType)value / m;
}

inline void ConvertSample(
    std::span<const std::byte> source,
    SampleType sourceType,
    std::span<std::byte> result,
    SampleType resultType)
{
    union {
        std::byte bytes[8];
        int8_t int8Value;
        int16_t int16Value;
        int32_t int24Value;
        int32_t int32Value;
        int64_t int64Value;
        float float32Value;
        double float64Value;
    } sample;

    if (sourceType == SampleType::Unspecified ||
        resultType == SampleType::Unspecified)
    {
        throw std::runtime_error("cannot convert to/from unspecified sample type");
    }

    assert(source.size() == GetBytesPerSample(sourceType));
    assert(result.size() == GetBytesPerSample(resultType));

    if (sourceType == SampleType::Int8)
    {
        std::copy(source.data(), source.data() + 1, sample.bytes);
        int8_t value = sample.int8Value;

        if (resultType == SampleType::Int8) {
            sample.int8Value = value;
            std::copy(sample.bytes, sample.bytes + 1, result.data());
        }
        else if (resultType == SampleType::Int16) {
            sample.int16Value = (int16_t)(0x100 * value);
            std::copy(sample.bytes, sample.bytes + 2, result.data());
        }
        else if (resultType == SampleType::Int24) {
            sample.int24Value = (int32_t)(0x10000 * value);
            std::copy(sample.bytes, sample.bytes + 3, result.data());
        }
        else if (resultType == SampleType::Int32) {
            sample.int32Value = (int32_t)(0x1000000 * value);
            std::copy(sample.bytes, sample.bytes + 4, result.data());
        }
        else if (resultType == SampleType::Int64) {
            sample.int64Value = (int64_t)(0x100000000000000 * value);
            std::copy(sample.bytes, sample.bytes + 8, result.data());
        }
        else if (resultType == SampleType::Float32) {
            sample.float32Value = IntToFloat<0x7F, float>(value);
            std::copy(sample.bytes, sample.bytes + 4, result.data());
        }
        else if (resultType == SampleType::Float64) {
            sample.float64Value = IntToFloat<0x7F, double>(value);
            std::copy(sample.bytes, sample.bytes + 8, result.data());
        }
    }
    else if (sourceType == SampleType::Int16)
    {
        std::copy(source.data(), source.data() + 2, sample.bytes);
        int16_t value = sample.int16Value;

        if (resultType == SampleType::Int8) {
            sample.int8Value = (int8_t)(value / 0x100);
            std::copy(sample.bytes, sample.bytes + 1, result.data());
        }
        else if (resultType == SampleType::Int16) {
            sample.int16Value = value;
            std::copy(sample.bytes, sample.bytes + 2, result.data());
        }
        else if (resultType == SampleType::Int24) {
            sample.int24Value = (int32_t)(0x100 * value);
            std::copy(sample.bytes, sample.bytes + 3, result.data());
        }
        else if (resultType == SampleType::Int32) {
            sample.int32Value = (int32_t)(0x10000 * value);
            std::copy(sample.bytes, sample.bytes + 4, result.data());
        }
        else if (resultType == SampleType::Int64) {
            sample.int64Value = (int64_t)(0x1000000000000 * value);
            std::copy(sample.bytes, sample.bytes + 8, result.data());
        }
        else if (resultType == SampleType::Float32) {
            sample.float32Value = IntToFloat<0x7FFF, float>(value);
            std::copy(sample.bytes, sample.bytes + 4, result.data());
        }
        else if (resultType == SampleType::Float64) {
            sample.float64Value = IntToFloat<0x7FFF, double>(value);
            std::copy(sample.bytes, sample.bytes + 8, result.data());
        }
    }
    else if (sourceType == SampleType::Int24)
    {
        std::copy(source.data(), source.data() + 3, sample.bytes);
        sample.bytes[3] = std::byte(0);
        int32_t value = sample.int24Value;

        if (resultType == SampleType::Int8) {
            sample.int8Value = (int8_t)(value / 0x10000);
            std::copy(sample.bytes, sample.bytes + 1, result.data());
        }
        else if (resultType == SampleType::Int16) {
            sample.int16Value = (int16_t)(value / 0x100);
            std::copy(sample.bytes, sample.bytes + 2, result.data());
        }
        else if (resultType == SampleType::Int24) {
            sample.int24Value = value;
            std::copy(sample.bytes, sample.bytes + 3, result.data());
        }
        else if (resultType == SampleType::Int32) {
            sample.int32Value = (int32_t)(0x100 * value);
            std::copy(sample.bytes, sample.bytes + 4, result.data());
        }
        else if (resultType == SampleType::Int64) {
            sample.int64Value = (int64_t)(0x10000000000 * value);
            std::copy(sample.bytes, sample.bytes + 8, result.data());
        }
        else if (resultType == SampleType::Float32) {
            sample.float32Value = IntToFloat<0x7FFFFF, float>(value);
            std::copy(sample.bytes, sample.bytes + 4, result.data());
        }
        else if (resultType == SampleType::Float64) {
            sample.float64Value = IntToFloat<0x7FFFFF, double>(value);
            std::copy(sample.bytes, sample.bytes + 8, result.data());
        }
    }
    else if (sourceType == SampleType::Int32)
    {
        std::copy(source.data(), source.data() + 4, sample.bytes);
        int32_t value = sample.int32Value;

        if (resultType == SampleType::Int8) {
            sample.int8Value = (int8_t)(value / 0x1000000);
            std::copy(sample.bytes, sample.bytes + 1, result.data());
        }
        else if (resultType == SampleType::Int16) {
            sample.int16Value = (int16_t)(value / 0x10000);
            std::copy(sample.bytes, sample.bytes + 2, result.data());
        }
        else if (resultType == SampleType::Int24) {
            sample.int24Value = (int32_t)(value / 0x100);
            std::copy(sample.bytes, sample.bytes + 3, result.data());
        }
        else if (resultType == SampleType::Int32) {
            sample.int32Value = value;
            std::copy(sample.bytes, sample.bytes + 4, result.data());
        }
        else if (resultType == SampleType::Int64) {
            sample.int64Value = (int64_t)(0x100000000 * value);
            std::copy(sample.bytes, sample.bytes + 8, result.data());
        }
        else if (resultType == SampleType::Float32) {
            sample.float32Value = IntToFloat<0x7FFFFFFF, float>(value);
            std::copy(sample.bytes, sample.bytes + 4, result.data());
        }
        else if (resultType == SampleType::Float64) {
            sample.float64Value = IntToFloat<0x7FFFFFFF, double>(value);
            std::copy(sample.bytes, sample.bytes + 8, result.data());
        }
    }
    else if (sourceType == SampleType::Int64)
    {
        std::copy(source.data(), source.data() + 8, sample.bytes);
        int64_t value = sample.int64Value;

        if (resultType == SampleType::Int8) {
            sample.int8Value = (int8_t)(value / 0x100000000000000);
            std::copy(sample.bytes, sample.bytes + 1, result.data());
        }
        else if (resultType == SampleType::Int16) {
            sample.int16Value = (int16_t)(value / 0x1000000000000);
            std::copy(sample.bytes, sample.bytes + 2, result.data());
        }
        else if (resultType == SampleType::Int24) {
            sample.int24Value = (int32_t)(value / 0x10000000000);
            std::copy(sample.bytes, sample.bytes + 3, result.data());
        }
        else if (resultType == SampleType::Int32) {
            sample.int32Value = (int32_t)(value / 0x100000000);
            std::copy(sample.bytes, sample.bytes + 4, result.data());
        }
        else if (resultType == SampleType::Int64) {
            sample.int64Value = value;
            std::copy(sample.bytes, sample.bytes + 8, result.data());
        }
        else if (resultType == SampleType::Float32) {
            sample.float32Value = IntToFloat<0x7FFFFFFFFFFFFFFF, float>(value);
            std::copy(sample.bytes, sample.bytes + 4, result.data());
        }
        else if (resultType == SampleType::Float64) {
            sample.float64Value = IntToFloat<0x7FFFFFFFFFFFFFFF, double>(value);
            std::copy(sample.bytes, sample.bytes + 8, result.data());
        }
    }
    else if (sourceType == SampleType::Float32)
    {
        std::copy(source.data(), source.data() + 4, sample.bytes);
        float value = sample.float32Value;

        if (resultType == SampleType::Int8) {
            sample.int8Value = FloatToInt<0x7F, int8_t>(value);
            std::copy(sample.bytes, sample.bytes + 1, result.data());
        }
        else if (resultType == SampleType::Int16) {
            sample.int16Value = FloatToInt<0x7FFF, int16_t>(value);
            std::copy(sample.bytes, sample.bytes + 2, result.data());
        }
        else if (resultType == SampleType::Int24) {
            sample.int24Value = FloatToInt<0x7FFFFF, int32_t>(value);
            std::copy(sample.bytes, sample.bytes + 3, result.data());
        }
        else if (resultType == SampleType::Int32) {
            sample.int32Value = FloatToInt<0x7FFFFFFF, int32_t>(value);
            std::copy(sample.bytes, sample.bytes + 4, result.data());
        }
        else if (resultType == SampleType::Int64) {
            sample.int64Value = FloatToInt<0x7FFFFFFFFFFFFFFF, int64_t>(value);
            std::copy(sample.bytes, sample.bytes + 8, result.data());
        }
        else if (resultType == SampleType::Float32) {
            sample.float32Value = value;
            std::copy(sample.bytes, sample.bytes + 4, result.data());
        }
        else if (resultType == SampleType::Float64) {
            sample.float64Value = (double)value;
            std::copy(sample.bytes, sample.bytes + 8, result.data());
        }
    }
    else if (sourceType == SampleType::Float64)
    {
        std::copy(source.data(), source.data() + 8, sample.bytes);
        double value = sample.float64Value;

        if (resultType == SampleType::Int8) {
            sample.int8Value = FloatToInt<0x7F, int8_t>(value);
            std::copy(sample.bytes, sample.bytes + 1, result.data());
        }
        else if (resultType == SampleType::Int16) {
            sample.int16Value = FloatToInt<0x7FFF, int16_t>(value);
            std::copy(sample.bytes, sample.bytes + 2, result.data());
        }
        else if (resultType == SampleType::Int24) {
            sample.int24Value = FloatToInt<0x7FFFFF, int32_t>(value);
            std::copy(sample.bytes, sample.bytes + 3, result.data());
        }
        else if (resultType == SampleType::Int32) {
            sample.int32Value = FloatToInt<0x7FFFFFFF, int32_t>(value);
            std::copy(sample.bytes, sample.bytes + 4, result.data());
        }
        else if (resultType == SampleType::Int64) {
            sample.int64Value = FloatToInt<0x7FFFFFFFFFFFFFFF, int64_t>(value);
            std::copy(sample.bytes, sample.bytes + 8, result.data());
        }
        else if (resultType == SampleType::Float32) {
            sample.float32Value = (float)value;
            std::copy(sample.bytes, sample.bytes + 4, result.data());
        }
        else if (resultType == SampleType::Float64) {
            sample.float64Value = value;
            std::copy(sample.bytes, sample.bytes + 8, result.data());
        }
    }
}

} // audio
} // mw
