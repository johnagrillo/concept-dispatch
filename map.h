#pragma once

#include "meta.h"
#include <array>
#include <string>
#include <map>
#include <stdexcept>

namespace meta {

// ============================================================================
// CONTAINERS MAP - Primary template (size inferred by compiler)
// ============================================================================

template<typename K, typename V, const auto& AllowedKeys>
class ContainersMap {
public:
    using key_type = K;
    using mapped_type = V;
    
    void insert(const K& key, const V& value) {
        // Validate key
        bool found = false;
        for (const auto& allowed : AllowedKeys) {
            if (key == allowed) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            std::string msg = "Key '" + key + "' not allowed. Valid keys: {";
            for (size_t i = 0; i < AllowedKeys.size(); i++) {
                if (i > 0) msg += ", ";
                msg += std::string(AllowedKeys[i]);
            }
            msg += "}";
            throw std::runtime_error(msg);
        }
        
        data_[key] = value;
    }

    size_t size() const { return data_.size(); }
    
    auto begin() { return data_.begin(); }
    auto end() { return data_.end(); }
    auto begin() const { return data_.begin(); }
    auto end() const { return data_.end(); }

private:
    std::map<K, V> data_;
};

// ============================================================================
// REGISTER CONTAINERS MAP WITH FRAMEWORK
// ============================================================================

template<typename K, typename V, const auto& AllowedKeys>
struct YamlTraits<ContainersMap<K, V, AllowedKeys>> {
    using type = ContainersMap<K, V, AllowedKeys>;
    
    static void parse(ContainersMap<K, V, AllowedKeys>& obj, const YAML::Node& node) {
        if (!node.IsMap()) {
            throw std::runtime_error("Expected map node for ContainersMap");
        }
        for (const auto& entry : node) {
            K key = entry.first.as<K>();
            V value = entry.second.as<V>();
            obj.insert(key, value);  // Validates key automatically
        }
    }
    
    static std::string toString(const ContainersMap<K, V, AllowedKeys>& obj) {
        std::string result = "{";
        bool first = true;
        for (const auto& [k, v] : obj) {
            if (!first) result += ", ";
            result += std::string(k) + "=" + std::string(v);
            first = false;
        }
        result += "}";
        return result;
    }
};

}  // namespace meta
