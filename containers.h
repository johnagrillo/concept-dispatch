#pragma once

#include <map>
#include <vector>
#include <concepts>
#include <stdexcept>

namespace meta {

// ============================================================================
// CONSTRAINED MAP
// ============================================================================

template<typename K, typename V, typename KeyConstraint, typename ValueConstraint>
class ConstrainedMap {
public:
    using key_type = K;
    using mapped_type = V;
    using value_type = std::pair<const K, V>;
    using container_type = std::map<K, V>;
    using iterator = typename container_type::iterator;
    using const_iterator = typename container_type::const_iterator;

    ConstrainedMap() = default;

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

    V& operator[](const K& key) {
        if (!KeyConstraint::validate(key)) {
            throw std::runtime_error(
                "Key constraint violated: " + KeyConstraint::error(key)
            );
        }
        return data_[key];
    }

    const V& at(const K& key) const {
        return data_.at(key);
    }

    bool contains(const K& key) const {
        return data_.count(key) > 0;
    }

    size_t size() const { return data_.size(); }
    bool empty() const { return data_.empty(); }

    iterator begin() { return data_.begin(); }
    iterator end() { return data_.end(); }
    const_iterator begin() const { return data_.begin(); }
    const_iterator end() const { return data_.end(); }

private:
    container_type data_;
};

// ============================================================================
// CONSTRAINED VECTOR
// ============================================================================

template<typename T, typename ElementConstraint>
class ConstrainedVector {
public:
    using value_type = T;
    using container_type = std::vector<T>;
    using iterator = typename container_type::iterator;
    using const_iterator = typename container_type::const_iterator;

    ConstrainedVector() = default;

    void push_back(const T& value) {
        if (!ElementConstraint::validate(value)) {
            throw std::runtime_error(
                "Element constraint violated: " + ElementConstraint::error(value)
            );
        }
        data_.push_back(value);
    }

    T& operator[](size_t index) {
        return data_[index];
    }

    const T& operator[](size_t index) const {
        return data_[index];
    }

    T& at(size_t index) {
        return data_.at(index);
    }

    const T& at(size_t index) const {
        return data_.at(index);
    }

    size_t size() const { return data_.size(); }
    bool empty() const { return data_.empty(); }

    iterator begin() { return data_.begin(); }
    iterator end() { return data_.end(); }
    const_iterator begin() const { return data_.begin(); }
    const_iterator end() const { return data_.end(); }

private:
    container_type data_;
};

// ============================================================================
// CONSTRAINT IMPLEMENTATIONS
// ============================================================================

struct NoConstraint {
    template <typename T>
    static bool validate(const T&) { return true; }
    template <typename T>
    static std::string error(const T&) { return "No constraint"; }
};

struct PositiveConstraint {
    static bool validate(int val) {
        return val > 0;
    }

    static std::string error(int val) {
        return "Value " + std::to_string(val) + " must be positive";
    }
};

struct NonEmptyStringConstraint {
    static bool validate(const std::string& s) {
        return !s.empty();
    }

    static std::string error(const std::string& s) {
        return "String cannot be empty";
    }
};

struct AnyIntConstraint {
    static bool validate(int val) {
        return true;
    }

    static std::string error(int val) {
        return "";
    }
};

struct AnyStringConstraint {
    static bool validate(const std::string& s) {
        return true;
    }

    static std::string error(const std::string& s) {
        return "";
    }
};

}  // namespace meta

