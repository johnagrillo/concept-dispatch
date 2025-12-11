#pragma once

#include "meta.h"
#include <array>
#include <vector>
#include <stdexcept>

namespace meta {

// ============================================================================
// WHITELISTED VECTOR - Like ContainersMap but for vectors
// ============================================================================

template<typename T, const auto& AllowedValues>
class WhitelistVector {
public:
    using value_type = T;
    
    void push_back(const T& value) {
        // Validate value
        bool found = false;
        for (const auto& allowed : AllowedValues) {
            if (value == allowed) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            std::string msg = "Value not allowed. Valid values: {";
            for (size_t i = 0; i < AllowedValues.size(); i++) {
                if (i > 0) msg += ", ";
                msg += std::string(AllowedValues[i]);
            }
            msg += "}";
            throw std::runtime_error(msg);
        }
        
        data_.push_back(value);
    }

    size_t size() const { return data_.size(); }
    bool empty() const { return data_.empty(); }
    
    auto begin() { return data_.begin(); }
    auto end() { return data_.end(); }
    auto begin() const { return data_.begin(); }
    auto end() const { return data_.end(); }
    
    const T& operator[](size_t i) const { return data_[i]; }
    T& operator[](size_t i) { return data_[i]; }

private:
    std::vector<T> data_;
};

// ============================================================================
// REGISTER WHITELISTED VECTOR WITH FRAMEWORK
// ============================================================================

template<typename T, const auto& AllowedValues>
struct YamlTraits<WhitelistVector<T, AllowedValues>> {
    using type = WhitelistVector<T, AllowedValues>;
    
    static void parse(WhitelistVector<T, AllowedValues>& obj, const YAML::Node& node) {
        if (!node.IsSequence()) {
            throw std::runtime_error("Expected sequence node for WhitelistVector");
        }
        for (const auto& item : node) {
            T value = item.as<T>();
            obj.push_back(value);  // Validates value automatically
        }
    }
    
    static std::string toString(const WhitelistVector<T, AllowedValues>& obj) {
        std::string result = "[";
        bool first = true;
        for (const auto& v : obj) {
            if (!first) result += ", ";
            result += std::string(v);
            first = false;
        }
        result += "]";
        return result;
    }
};

}  // namespace meta
