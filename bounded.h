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
    
    // Default constructor - no validation, just initializes to Min
    BoundedInt(int v = Min) : val(v) {}
    
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
    
    // Default constructor - creates valid default string of MinLen spaces
    BoundedString() : val(std::string(MinLen, ' ')) {}
    
    // Parameterized constructor - no validation, just assigns
    BoundedString(const std::string& v) : val(v) {}
    
    static constexpr size_t minLen = MinLen;
    static constexpr size_t maxLen = MaxLen;
    
    bool isValid() const { return val.length() >= MinLen && val.length() <= MaxLen; }
};

// ============================================================================
// REGISTER BOUNDED TYPES WITH FRAMEWORK
// ============================================================================

// Register BoundedInt as YamlSerializable with validation in parse
template<int Min, int Max>
struct YamlTraits<BoundedInt<Min, Max>> {
    using type = BoundedInt<Min, Max>;
    
    static ValidationResult parse(BoundedInt<Min, Max>& obj, const YAML::Node& node) {
        int value;
        try {
            value = node.template as<int>();
        } catch (const std::exception& e) {
            ValidationResult result;
            result.addError("", std::string("Invalid integer: ") + e.what());
            return result;
        }
        
        if (value < Min || value > Max) {
            ValidationResult result;
            result.addError("", 
                "Value " + std::to_string(value) + " out of bounds [" + 
                std::to_string(Min) + ", " + std::to_string(Max) + "]"
            );
            return result;
        }
        
        obj.val = value;
        return ValidationResult();
    }
    
    static std::string toString(const BoundedInt<Min, Max>& obj) {
        return std::to_string(obj.val);
    }
};

// Register BoundedString as YamlSerializable with validation in parse
template<size_t MinLen, size_t MaxLen>
struct YamlTraits<BoundedString<MinLen, MaxLen>> {
    using type = BoundedString<MinLen, MaxLen>;
    
    static ValidationResult parse(BoundedString<MinLen, MaxLen>& obj, const YAML::Node& node) {
        std::string value;
        try {
            value = node.template as<std::string>();
        } catch (const std::exception& e) {
            ValidationResult result;
            result.addError("", std::string("Invalid string: ") + e.what());
            return result;
        }
        
        if (value.length() < MinLen || value.length() > MaxLen) {
            ValidationResult result;
            result.addError("", 
                "String length " + std::to_string(value.length()) + 
                " out of bounds [" + std::to_string(MinLen) + ", " + 
                std::to_string(MaxLen) + "]"
            );
            return result;
        }
        
        obj.val = value;
        return ValidationResult();
    }
    
    static std::string toString(const BoundedString<MinLen, MaxLen>& obj) {
        return obj.val;
    }
};

} // namespace meta

