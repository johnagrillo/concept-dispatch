#pragma once

#include <map>
#include <optional>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>
#include <yaml-cpp/yaml.h>
#include <sstream>

namespace meta {

// ============================================================================
// FORWARD DECLARATIONS
// ============================================================================

template <typename T> struct YamlTraits;

// ============================================================================
// HELPER: Check if YamlTraits<T> is specialized
// ============================================================================

template <typename T, typename = void> 
inline constexpr bool has_yaml_traits_v = false;

template <typename T>
inline constexpr bool has_yaml_traits_v<T, std::void_t<typename YamlTraits<T>::type>> = true;

// ============================================================================
// TYPE CONCEPTS
// ============================================================================

template <typename T>
concept HasYamlTraits = has_yaml_traits_v<T>;

template <typename T>
concept HasFields = requires(T t) {
    { T::fields };
};

// ============================================================================
// UTILITIES
// ============================================================================

enum class Requirement : uint8_t {
    Required,
    Optional
};

constexpr auto RequiredField = Requirement::Required;
constexpr auto OptionalField = Requirement::Optional;

template <typename MemberPtr> 
struct member_pointer_traits;

template <typename T, typename Class> 
struct member_pointer_traits<T Class::*> {
    using type = T;
};

struct ValidationResult {
    bool valid = true;
    std::vector<std::pair<std::string, std::string>> errors;

    void addError(std::string_view fieldName, std::string_view message) {
        valid = false;
        errors.push_back({std::string(fieldName), std::string(message)});
    }

    void mergeErrors(std::string_view fieldName, const ValidationResult& other) {
        if (!other.valid) {
            valid = false;
            for (const auto& [errField, errMsg] : other.errors) {
                errors.push_back({std::string(fieldName) + "." + errField, errMsg});
            }
        }
    }
};

// ============================================================================
// YAML TRAITS FOR PRIMITIVES
// ============================================================================

template <> 
struct YamlTraits<std::string> {
    using type = std::string;
    static ValidationResult parse(std::string& obj, const YAML::Node& node) {
        try {
            obj = node.template as<std::string>();
        } catch (const std::exception& e) {
            ValidationResult result;
            result.addError("", std::string("Invalid string: ") + e.what());
            return result;
        }
        return ValidationResult();
    }
    static std::string toString(const std::string& obj) {
        return obj;
    }
};

template <> 
struct YamlTraits<int> {
    using type = int;
    static ValidationResult parse(int& obj, const YAML::Node& node) {
        try {
            obj = node.template as<int>();
        } catch (const std::exception& e) {
            ValidationResult result;
            result.addError("", std::string("Invalid integer: ") + e.what());
            return result;
        }
        return ValidationResult();
    }
    static std::string toString(const int& obj) {
        return std::to_string(obj);
    }
};

template <> 
struct YamlTraits<double> {
    using type = double;
    static ValidationResult parse(double& obj, const YAML::Node& node) {
        try {
            obj = node.template as<double>();
        } catch (const std::exception& e) {
            ValidationResult result;
            result.addError("", std::string("Invalid double: ") + e.what());
            return result;
        }
        return ValidationResult();
    }
    static std::string toString(const double& obj) {
        return std::to_string(obj);
    }
};

template <> 
struct YamlTraits<bool> {
    using type = bool;
    static ValidationResult parse(bool& obj, const YAML::Node& node) {
        try {
            obj = node.template as<bool>();
        } catch (const std::exception& e) {
            ValidationResult result;
            result.addError("", std::string("Invalid boolean: ") + e.what());
            return result;
        }
        return ValidationResult();
    }
    static std::string toString(const bool& obj) {
        return obj ? "true" : "false";
    }
};

// ============================================================================
// YAML TRAITS FOR STD CONTAINERS
// ============================================================================

template <> 
struct YamlTraits<std::map<std::string, std::string>> {
    using type = std::map<std::string, std::string>;
    static ValidationResult parse(std::map<std::string, std::string>& obj, const YAML::Node& node) {
        try {
            obj = node.template as<std::map<std::string, std::string>>();
        } catch (const std::exception& e) {
            ValidationResult result;
            result.addError("", std::string("Invalid map: ") + e.what());
            return result;
        }
        return ValidationResult();
    }
    static std::string toString(const std::map<std::string, std::string>& obj) {
        std::string result = "{";
        bool first = true;
        for (const auto& [k, v] : obj) {
            if (!first) result += ", ";
            result += k + "=" + v;
            first = false;
        }
        result += "}";
        return result;
    }
};

template <> 
struct YamlTraits<std::vector<std::string>> {
    using type = std::vector<std::string>;
    static ValidationResult parse(std::vector<std::string>& obj, const YAML::Node& node) {
        try {
            obj = node.template as<std::vector<std::string>>();
        } catch (const std::exception& e) {
            ValidationResult result;
            result.addError("", std::string("Invalid vector: ") + e.what());
            return result;
        }
        return ValidationResult();
    }
    static std::string toString(const std::vector<std::string>& obj) {
        std::string result = "[";
        bool first = true;
        for (const auto& item : obj) {
            if (!first) result += ", ";
            result += item;
            first = false;
        }
        result += "]";
        return result;
    }
};

// ============================================================================
// DISPATCH FUNCTIONS
// ============================================================================

template <HasYamlTraits T> 
ValidationResult dispatchParse(T& obj, const YAML::Node& node) {
    return YamlTraits<T>::parse(obj, node);
}

template <HasYamlTraits T> 
std::string dispatchToString(const T& obj) {
    return YamlTraits<T>::toString(obj);
}

// ============================================================================
// Field Definition
// ============================================================================

template <auto MemberPtr> 
struct Field {
    using type = typename member_pointer_traits<decltype(MemberPtr)>::type;

    std::string_view fieldName;
    std::string_view fieldDesc;
    Requirement requirement;
    static constexpr auto memberPtr = MemberPtr;

    constexpr Field(std::string_view name, std::string_view desc, Requirement req)
        : fieldName(name), fieldDesc(desc), requirement(req) {}
};

// ============================================================================
// PARSING FUNCTIONS
// ============================================================================

template <HasFields T> 
std::optional<T> fromYaml(const YAML::Node& yaml) {
    T obj{};

    std::apply(
        [&](auto&&... fields) {
            (..., [&](auto& field) {
                if (!yaml[field.fieldName]) return;

                const auto& fieldNode = yaml[field.fieldName];
                auto parseResult = dispatchParse(obj.*field.memberPtr, fieldNode);
                // Silently skip on error for non-validating version
            }(fields));
        },
        T::fields);

    return obj;
}

template <HasFields T>
std::pair<std::optional<T>, ValidationResult> fromYamlWithValidation(const YAML::Node& yaml) {
    T obj{};
    ValidationResult result;

    std::apply(
        [&](auto&&... fields) {
            (..., [&](auto& field) {
                if (!yaml[field.fieldName]) {
                    if (field.requirement == Requirement::Required) {
                        result.addError(field.fieldName, "Missing required field");
                    }
                    return;
                }

                const auto& fieldNode = yaml[field.fieldName];
                auto parseResult = dispatchParse(obj.*field.memberPtr, fieldNode);
                if (!parseResult.valid) {
                    result.mergeErrors(field.fieldName, parseResult);
                }
            }(fields));
        },
        T::fields);

    if (result.valid) {
        return {obj, result};
    } else {
        return {std::nullopt, result};
    }
}

template <HasFields T> 
std::string toString(const T& obj) {
    std::ostringstream oss;

    std::apply(
        [&](auto&&... fields) {
            (..., [&](auto& field) {
                auto value = const_cast<T&>(obj).*field.memberPtr;
                oss << field.fieldName << ": " << dispatchToString(value);
                if (!field.fieldDesc.empty())
                    oss << "  # " << field.fieldDesc;
                oss << "\n";
            }(fields));
        },
        T::fields);

    return oss.str();
}

template <HasFields T> 
std::map<std::string, std::string> toYamlMap(const T& obj) {
    std::map<std::string, std::string> result;

    std::apply(
        [&](auto&&... fields) {
            (..., [&](auto& field) {
                auto value = const_cast<T&>(obj).*field.memberPtr;
                result[std::string(field.fieldName)] = dispatchToString(value);
            }(fields));
        },
        T::fields);

    return result;
}

template <HasFields T> 
std::string toJson(const T& obj) {
    std::ostringstream oss;
    oss << "{\n";

    bool first = true;
    std::apply(
        [&](auto&&... fields) {
            (..., [&](auto& field) {
                if (!first) oss << ",\n";
                first = false;

                auto value = const_cast<T&>(obj).*field.memberPtr;
                oss << "  \"" << field.fieldName << "\": ";

                if constexpr (std::is_same_v<typename decltype(field)::type, std::string>) {
                    oss << "\"" << dispatchToString(value) << "\"";
                } else if constexpr (std::is_integral_v<typename decltype(field)::type> ||
                                     std::is_floating_point_v<typename decltype(field)::type> ||
                                     std::is_same_v<typename decltype(field)::type, bool>) {
                    oss << dispatchToString(value);
                } else {
                    oss << "\"" << dispatchToString(value) << "\"";
                }
            }(fields));
        },
        T::fields);

    oss << "\n}\n";
    return oss.str();
}

}  // namespace meta

