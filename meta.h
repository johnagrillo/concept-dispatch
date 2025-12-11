#pragma once

#include <map>
#include <optional>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>
#include <yaml-cpp/yaml.h>
#include <type_traits>
#include <array>
#include <cstdint>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>


namespace meta
{
//
//   enum class Environment { Development, Staging, Production };
//
//   namespace bhw {
//   template <>
//   struct EnumMapping<Environment> {
//       static constexpr std::array mapping = std::array{
//           std::pair(Environment::Development, "dev"),
//           std::pair(Environment::Staging, "staging"),
//           std::pair(Environment::Production, "prod"),
//       };
//       using Type = EnumTraitsAuto<Environment, mapping>;
//   };
//   } // namespace bhw
//
// Then use in field:
//   meta::Field<&Config::env>{"env", "Environment", meta::Required,
//   meta::EnumConstraint<Environment>()}
//

template <typename EnumT, auto& MappingArray> struct EnumTraitsAuto
{
    inline static constexpr auto& mapping = MappingArray;

    inline static const std::unordered_map<EnumT, std::string> enumToString = []
    {
        std::unordered_map<EnumT, std::string> m;
        for (auto [e, s] : mapping)
            m[e] = s;
        return m;
    }();

    inline static const std::unordered_map<std::string, EnumT> stringToEnum = []
    {
        std::unordered_map<std::string, EnumT> m;
        for (auto [e, s] : mapping)
            m[s] = e;
        return m;
    }();

    static std::string toString(EnumT e)
    {
        auto it = enumToString.find(e);
        return it != enumToString.end() ? it->second : "";
    }

    static std::optional<EnumT> fromString(const std::string& s)
    {
        auto it = stringToEnum.find(s);
        return it != stringToEnum.end() ? std::optional(it->second) : std::nullopt;
    }

    template <typename Func> static void forEach(Func f)
    {
        for (auto [e, _] : mapping)
            f(e);
    }
};



template <typename EnumT> struct EnumMapping; // specialization per enum

template <typename T>
concept RegisteredEnum = requires { typename EnumMapping<T>::Type; };

template <RegisteredEnum EnumT> std::ostream& operator<<(std::ostream& os, EnumT e)
{
    using Traits = typename EnumMapping<EnumT>::Type;
    os << Traits::toString(e);
    return os;
}

template <typename EnumT> std::optional<EnumT> to_enum(const std::string& s)
{
    return EnumMapping<EnumT>::Type::fromString(s);
}

template <typename EnumT> EnumT to_enum_checked(const std::string& s)
{
    auto e = EnumMapping<EnumT>::Type::fromString(s);
    if (!e)
        throw std::runtime_error("Invalid enum string: " + s);
    return *e;
}

// Forward declaration
template <typename T> struct YamlTraits;

// ============================================================================
// HELPER: Check if YamlTraits<T> is specialized
// ============================================================================

template <typename T, typename = void> inline constexpr bool has_yaml_traits_v = false;

template <typename T>
inline constexpr bool has_yaml_traits_v<T, std::void_t<typename YamlTraits<T>::type>> = true;

// ============================================================================
// TYPE CONCEPTS
// ============================================================================

template <typename T>
concept HasYamlTraits = has_yaml_traits_v<T>;

template <typename T>
concept IsEnum = std::is_enum_v<T>;

template <typename T>
concept HasFields = requires(T t) {
    { T::fields };
};

// ============================================================================
// UTILITIES
// ============================================================================

enum class FieldType : uint8_t
{
    String,
    Integer,
    Float,
    Boolean,
    Enum,
    Map,
    Array,
    Optional,
    Custom,
    Unknown
};

enum class Requirement : uint8_t
{
    Required,
    Optional
};

constexpr auto RequiredField = Requirement::Required;
constexpr auto OptionalField = Requirement::Optional;

template <typename MemberPtr> struct member_pointer_traits;

template <typename T, typename Class> struct member_pointer_traits<T Class::*>
{
    using type = T;
};

struct ValidationResult
{
    bool valid = true;
    std::vector<std::pair<std::string, std::string>> errors;

    void addError(std::string_view fieldName, std::string_view message)
    {
        valid = false;
        errors.push_back({std::string(fieldName), std::string(message)});
    }
};

// ============================================================================
// YAML TYPE TRAITS - Add new types here
// ============================================================================

template <typename T> struct YamlTraits;

template <> struct YamlTraits<std::string>
{
    using type = std::string;
    static void parse(std::string& obj, const YAML::Node& node)
    {
        obj = node.template as<std::string>();
    }
    static std::string toString(const std::string& obj)
    {
        return obj;
    }
};

template <> struct YamlTraits<int>
{
    using type = int;
    static void parse(int& obj, const YAML::Node& node)
    {
        obj = node.template as<int>();
    }
    static std::string toString(const int& obj)
    {
        return std::to_string(obj);
    }
};

template <> struct YamlTraits<double>
{
    using type = double;
    static void parse(double& obj, const YAML::Node& node)
    {
        obj = node.template as<double>();
    }
    static std::string toString(const double& obj)
    {
        return std::to_string(obj);
    }
};

template <> struct YamlTraits<bool>
{
    using type = bool;
    static void parse(bool& obj, const YAML::Node& node)
    {
        obj = node.template as<bool>();
    }
    static std::string toString(const bool& obj)
    {
        return obj ? "true" : "false";
    }
};

template <> struct YamlTraits<std::map<std::string, std::string>>
{
    using type = std::map<std::string, std::string>;
    static void parse(std::map<std::string, std::string>& obj, const YAML::Node& node)
    {
        obj = node.template as<std::map<std::string, std::string>>();
    }
    static std::string toString(const std::map<std::string, std::string>& obj)
    {
        std::string result;
        bool first = true;
        for (const auto& [k, v] : obj)
        {
            if (!first)
                result += ",";
            result += k + "=" + v;
            first = false;
        }
        return result;
    }
};

template <> struct YamlTraits<std::vector<std::string>>
{
    using type = std::vector<std::string>;
    static void parse(std::vector<std::string>& obj, const YAML::Node& node)
    {
        obj = node.template as<std::vector<std::string>>();
    }
    static std::string toString(const std::vector<std::string>& obj)
    {
        std::string result;
        bool first = true;
        for (const auto& item : obj)
        {
            if (!first)
                result += ",";
            result += item;
            first = false;
        }
        return result;
    }
};

// ============================================================================
// DISPATCH FUNCTIONS - Compiler picks the right overload!
// ============================================================================

template <HasYamlTraits T> void dispatchParse(T& obj, const YAML::Node& node)
{
    YamlTraits<T>::parse(obj, node);
}

template <IsEnum T> void dispatchParse(T& obj, const YAML::Node& node)
{
    if constexpr (requires { typename EnumMapping<T>::Type; })
    {
        using Traits = typename EnumMapping<T>::Type;
        auto val = Traits::fromString(node.template as<std::string>());
        if (val)
            obj = val.value();
    }
}

template <HasYamlTraits T> std::string dispatchToString(const T& obj)
{
    return YamlTraits<T>::toString(obj);
}

template <IsEnum T> std::string dispatchToString(const T& obj)
{
    if constexpr (requires { typename EnumMapping<T>::Type; })
    {
        using Traits = typename EnumMapping<T>::Type;
        return Traits::toString(obj);
    }
    return "enum";
}

// ============================================================================
// Field Definition
// ============================================================================

template <auto MemberPtr> struct Field
{
    using type = typename member_pointer_traits<decltype(MemberPtr)>::type;

    std::string_view fieldName;
    std::string_view fieldDesc;
    Requirement requirement;
    static constexpr auto memberPtr = MemberPtr;

    constexpr Field(std::string_view name, std::string_view desc, Requirement req)
        : fieldName(name),
          fieldDesc(desc),
          requirement(req)
    {
    }
};

// ============================================================================
// PARSING FUNCTIONS - No if constexpr chains!
// ============================================================================

template <HasFields T> std::optional<T> fromYaml(const YAML::Node& yaml)
{
    T obj{};

    std::apply(
        [&](auto&&... fields)
        {
            (...,
             [&](auto& field)
             {
                 if (!yaml[field.fieldName])
                 {
                     return;
                 }

                 const auto& fieldNode = yaml[field.fieldName];
                 using MemberType = typename std::remove_cvref_t<decltype(field)>::type;

                 try
                 {
                     dispatchParse(obj.*field.memberPtr, fieldNode);
                 }
                 catch (const std::exception& e)
		 {
                     // Silently skip
                 }
                 catch (...)
                 {
                     // Catch anything
                 }
             }(fields));
        },
        T::fields);

    return obj;
}



  template <HasFields T>
std::pair<std::optional<T>, ValidationResult> fromYamlWithValidation(const YAML::Node& yaml)
{
    T obj{};
    ValidationResult result;

    std::apply(
        [&](auto&&... fields)
        {
            (...,
             [&](auto& field)
             {
                 if (!yaml[field.fieldName])
                 {
                     if (field.requirement == Requirement::Required)
                     {
                         result.addError(field.fieldName, "Missing required field");
                     }
                     return;
                 }

                 const auto& fieldNode = yaml[field.fieldName];
                 using MemberType = typename std::remove_cvref_t<decltype(field)>::type;

                 try
                 {
                     dispatchParse(obj.*field.memberPtr, fieldNode);
                 }
                 catch (const std::exception& e)
                 {
                     result.addError(field.fieldName, std::string("Parse error: ") + e.what());
                 }
                 catch (...)
                 {
                     result.addError(field.fieldName, "Unknown parse error");
                 }
             }(fields));
        },
        T::fields);

    if (result.valid)
    {
        return {obj, result};
    }
    else
    {
        return {std::nullopt, result};
    }
}


template <HasFields T> std::string toString(const T& obj)
{
    std::ostringstream oss;

    std::apply(
        [&](auto&&... fields)
        {
            (...,
             [&](auto& field)
             {
                 using MemberType = typename std::remove_cvref_t<decltype(field)>::type;
                 auto value = const_cast<T&>(obj).*field.memberPtr;
                 oss << field.fieldName << ": ";

                 // NO if constexpr chain - just dispatch!
                 oss << dispatchToString(value);

                 if (!field.fieldDesc.empty())
                     oss << "  # " << field.fieldDesc;
                 oss << "\n";
             }(fields));
        },
        T::fields);

    return oss.str();
}

template <HasFields T> std::map<std::string, std::string> toYamlMap(const T& obj)
{
    std::map<std::string, std::string> result;

    std::apply(
        [&](auto&&... fields)
        {
            (...,
             [&](auto& field)
             {
                 using MemberType = typename std::remove_cvref_t<decltype(field)>::type;
                 auto value = const_cast<T&>(obj).*field.memberPtr;
                 result[std::string(field.fieldName)] = dispatchToString(value);
             }(fields));
        },
        T::fields);

    return result;
}

template <HasFields T> std::string toJson(const T& obj)
{
    std::ostringstream oss;
    oss << "{\n";

    bool first = true;
    std::apply(
        [&](auto&&... fields)
        {
            (...,
             [&](auto& field)
             {
                 using MemberType = typename std::remove_cvref_t<decltype(field)>::type;
                 if (!first)
                     oss << ",\n";
                 first = false;

                 auto value = const_cast<T&>(obj).*field.memberPtr;
                 oss << "  \"" << field.fieldName << "\": ";

                 // Quote strings
                 if constexpr (std::is_same_v<MemberType, std::string>)
                 {
                     oss << "\"" << dispatchToString(value) << "\"";
                 }
                 else if constexpr (std::is_integral_v<MemberType> ||
                                    std::is_floating_point_v<MemberType> ||
                                    std::is_same_v<MemberType, bool>)
                 {
                     oss << dispatchToString(value);
                 }
                 else
                 {
                     oss << "\"" << dispatchToString(value) << "\"";
                 }
             }(fields));
        },
        T::fields);

    oss << "\n}\n";
    return oss.str();
}



  
} // namespace meta


