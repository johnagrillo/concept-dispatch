#pragma once

#include <map>
#include <vector>
#include <concepts>
#include <stdexcept>
#include <string>

namespace meta {

// ============================================================================
// CONSTRAINED MAP
// ============================================================================

template<typename K, typename V, typename KeyConstraint, typename ValueConstraint>
class ConstrainedMap {
public:
    using key_type = K;
    using mapped_type = V;

    void insert(const K& key, const V& value) {
        if (!KeyConstraint::validate(key)) {
            throw std::runtime_error(
                "Key constraint violated: " + KeyConstraint::error(key)
            );
        }
        if (!ValueConstraint::validate(value)) {
            throw std::runtime_error(
                "Value constraint violated: " + ValueConstraint::error(value)
            );
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
// CONSTRAINED VECTOR
// ============================================================================

template<typename T, typename ElementConstraint>
class ConstrainedVector {
public:
    void push_back(const T& value) {
        if (!ElementConstraint::validate(value)) {
            throw std::runtime_error(
                "Element constraint violated: " + ElementConstraint::error(value)
            );
        }
        data_.push_back(value);
    }

    T& operator[](size_t index) { return data_[index]; }
    const T& operator[](size_t index) const { return data_[index]; }

    size_t size() const { return data_.size(); }

    auto begin() { return data_.begin(); }
    auto end() { return data_.end(); }
    auto begin() const { return data_.begin(); }
    auto end() const { return data_.end(); }

private:
    std::vector<T> data_;
};

// ============================================================================
// CONSTRAINTS
// ============================================================================

struct PositiveConstraint {
    static bool validate(int val) { return val > 0; }
    static std::string error(int val) {
        return "Value " + std::to_string(val) + " must be positive";
    }
};

struct NonEmptyStringConstraint {
    static bool validate(const std::string& s) { return !s.empty(); }
    static std::string error(const std::string& s) {
        return "String cannot be empty";
    }
};

struct AnyIntConstraint {
    static bool validate(int val) { return true; }
    static std::string error(int val) { return ""; }
};

struct AnyStringConstraint {
    static bool validate(const std::string& s) { return true; }
    static std::string error(const std::string& s) { return ""; }
};

// ============================================================================
// WHITELIST CONSTRAINTS
// ============================================================================

class StringKeyWhitelist {
private:
    std::vector<std::string> allowedKeys_;

public:
    StringKeyWhitelist(const std::vector<std::string>& keys) : allowedKeys_(keys) {}

    bool validate(const std::string& key) const {
        for (const auto& allowed : allowedKeys_) {
            if (key == allowed) return true;
        }
        return false;
    }

    std::string error(const std::string& key) const {
        return "Key '" + key + "' not in whitelist";
    }
};

class IntValueWhitelist {
private:
    std::vector<int> allowedValues_;

public:
    IntValueWhitelist(const std::vector<int>& values) : allowedValues_(values) {}

    bool validate(int value) const {
        for (auto allowed : allowedValues_) {
            if (value == allowed) return true;
        }
        return false;
    }

    std::string error(int value) const {
        return "Value " + std::to_string(value) + " not in whitelist";
    }
};

class StringValueWhitelist {
private:
    std::vector<std::string> allowedValues_;

public:
    StringValueWhitelist(const std::vector<std::string>& values) : allowedValues_(values) {}

    bool validate(const std::string& value) const {
        for (const auto& allowed : allowedValues_) {
            if (value == allowed) return true;
        }
        return false;
    }

    std::string error(const std::string& value) const {
        return "Value '" + value + "' not in whitelist";
    }
};

}  // namespace meta
