#pragma once
#include "meta.h"
#include <string>

namespace meta {

// ============================================================================
// USER-DEFINED BOUNDED INT (external to framework)
// ============================================================================

template<int Min, int Max>
struct BoundedInt {
    int val;
    
    BoundedInt(int v = Min) : val(v) {
        if (v < Min || v > Max) {
            throw std::runtime_error(
                "Value " + std::to_string(v) + " out of bounds [" + 
                std::to_string(Min) + ", " + std::to_string(Max) + "]"
            );
        }
    }
    
    static constexpr int min = Min;
    static constexpr int max = Max;
    
    bool isValid() const { return val >= Min && val <= Max; }
};

// ============================================================================
// USER-DEFINED BOUNDED STRING (external to framework)
// ============================================================================

template<size_t MinLen, size_t MaxLen>
struct BoundedString {
    std::string val;
    
    // Default constructor - creates valid default string
    BoundedString() : val(std::string(MinLen, ' ')) { }
    
    // Parameterized constructor - validates input
    BoundedString(const std::string& v) : val(v) {
        if (v.length() < MinLen || v.length() > MaxLen) {
            throw std::runtime_error(
                "String length " + std::to_string(v.length()) + 
                " out of bounds [" + std::to_string(MinLen) + ", " + 
                std::to_string(MaxLen) + "]"
            );
        }
    }
    
    static constexpr size_t minLen = MinLen;
    static constexpr size_t maxLen = MaxLen;
    
    bool isValid() const { return val.length() >= MinLen && val.length() <= MaxLen; }
};

// ============================================================================
// REGISTER BOUNDED TYPES WITH FRAMEWORK
// ============================================================================

// Register BoundedInt as YamlSerializable
template<int Min, int Max>
struct YamlTraits<BoundedInt<Min, Max>> {
    using type = BoundedInt<Min, Max>;
    
    static void parse(BoundedInt<Min, Max>& obj, const YAML::Node& node) {
        obj = BoundedInt<Min, Max>(node.template as<int>());
    }
    
    static std::string toString(const BoundedInt<Min, Max>& obj) {
        return std::to_string(obj.val);
    }
};

// Register BoundedString as YamlSerializable
template<size_t MinLen, size_t MaxLen>
struct YamlTraits<BoundedString<MinLen, MaxLen>> {
    using type = BoundedString<MinLen, MaxLen>;
    
    static void parse(BoundedString<MinLen, MaxLen>& obj, const YAML::Node& node) {
        obj = BoundedString<MinLen, MaxLen>(node.template as<std::string>());
    }
    
    static std::string toString(const BoundedString<MinLen, MaxLen>& obj) {
        return obj.val;
    }
};

} // namespace meta

