


#include "meta.h"
#include "bounded.h"
#include <iostream>

// ============================================================================
// USER STRUCT WITH BOUNDED TYPES
// ============================================================================

struct Person {
  meta::BoundedString<1, 100> name;
    meta::BoundedInt<0, 150> age;
    meta::BoundedInt<0, 100> score;
    
    static constexpr auto fields = std::tuple{
        meta::Field<&Person::name>("name", "Person's name", meta::RequiredField),
        meta::Field<&Person::age>("age", "Person's age (0-150)", meta::RequiredField),
        meta::Field<&Person::score>("score", "Person's score (0-100)", meta::RequiredField)
    };
};

struct AppConfig {
  meta::BoundedString<1, 255> hostname;
  meta::BoundedInt<1, 65535> port;
  std::map<std::string, std::string> settings;
    
    static constexpr auto fields = std::tuple{
        meta::Field<&AppConfig::hostname>("hostname", "Server hostname", meta::RequiredField),
        meta::Field<&AppConfig::port>("port", "Server port", meta::RequiredField),
        meta::Field<&AppConfig::settings>("settings", "Settings map", meta::RequiredField)
    };
};

// ============================================================================
// MAIN
// ============================================================================

int main() {
    std::cout << "=== Concept-Based Dispatch Example ===\n\n";
    
    // ========================================
    // Example 1: Simple Person
    // ========================================
    std::cout << "--- Example 1: Person ---\n";
    
    YAML::Node person_yaml = YAML::Load(R"(
        name: Alice Johnson
        age: 28
        score: 95
    )");
    
    auto person = meta::fromYaml<Person>(person_yaml);
    if (person) {
        std::cout << "✓ Parsed successfully\n";
        std::cout << "\ntoString():\n" << meta::toString(*person);
        std::cout << "\ntoJson():\n" << meta::toJson(*person);
    }
    
    // ========================================
    // Example 2: Validation - out of bounds
    // ========================================
    std::cout << "\n--- Example 2: Validation (Out of Bounds) ---\n";
    
    YAML::Node invalid_person = YAML::Load(R"(
        name: Bob
        age: 200
        score: 95
    )");
    
    auto [invalid, result] = meta::fromYamlWithValidation<Person>(invalid_person);
    if (!invalid) {
        std::cout << "✗ Validation failed:\n";
        for (const auto& [field, error] : result.errors) {
            std::cout << "  " << field << ": " << error << "\n";
        }
    }
    
    // ========================================
    // Example 3: String too long
    // ========================================
    std::cout << "\n--- Example 3: String Constraint ---\n";
    
    YAML::Node long_name = YAML::Load(R"(
        name: "This is a very very very very very very very very very very very very long name that exceeds 100 characters"
        age: 30
        score: 85
    )");
    
    auto [person3, result3] = meta::fromYamlWithValidation<Person>(long_name);
    if (!person3) {
        std::cout << "✗ Name too long:\n";
        for (const auto& [field, error] : result3.errors) {
            std::cout << "  " << field << ": " << error << "\n";
        }
    }
    
    // ========================================
    // Example 4: AppConfig
    // ========================================
    std::cout << "\n--- Example 4: AppConfig ---\n";
    
    YAML::Node config_yaml = YAML::Load(R"(
        hostname: api.example.com
        port: 8080
        settings:
            timeout: "30"
            retries: "3"
            debug: "false"
    )");
    
    auto config = meta::fromYaml<AppConfig>(config_yaml);
    if (config) {
        std::cout << "✓ Config parsed successfully\n";
        std::cout << "\ntoString():\n" << meta::toString(*config);
        std::cout << "\ntoJson():\n" << meta::toJson(*config);
    }
    
    // ========================================
    // Example 5: Invalid port
    // ========================================
    std::cout << "\n--- Example 5: Port Out of Range ---\n";
    
    YAML::Node bad_config = YAML::Load(R"(
        hostname: localhost
        port: 99999
        settings:
            key: value
    )");
    
    auto [config5, result5] = meta::fromYamlWithValidation<AppConfig>(bad_config);
    if (!config5) {
        std::cout << "✗ Invalid config:\n";
        for (const auto& [field, error] : result5.errors) {
            std::cout << "  " << field << ": " << error << "\n";
        }
    }
    
    // ========================================
    // Example 6: Missing required field
    // ========================================
    std::cout << "\n--- Example 6: Missing Required Field ---\n";
    
    YAML::Node incomplete = YAML::Load(R"(
        name: Charlie
        age: 35
    )");
    
    auto [person6, result6] = meta::fromYamlWithValidation<Person>(incomplete);
    if (!person6) {
        std::cout << "✗ Missing fields:\n";
        for (const auto& [field, error] : result6.errors) {
            std::cout << "  " << field << ": " << error << "\n";
        }
    }
    
    // ========================================
    // Example 7: Valid config with validation
    // ========================================
    std::cout << "\n--- Example 7: Valid with Validation ---\n";
    
    YAML::Node good_config = YAML::Load(R"(
        hostname: production.example.com
        port: 443
        settings:
            ssl: "true"
            compression: "gzip"
    )");
    
    auto [config7, result7] = meta::fromYamlWithValidation<AppConfig>(good_config);
    if (config7) {
        std::cout << "✓ Valid config:\n" << meta::toString(*config7);
    }
    
    std::cout << "\n=== Done ===\n";
    return 0;
}
