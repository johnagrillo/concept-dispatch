#include "meta.h"
#include "map.h"
#include <iostream>

// ============================================================================
// COMPILE-TIME WHITELISTS
// ============================================================================

constexpr std::array config_keys = std::array{
    std::string_view("hostname"),
    std::string_view("port"),
    std::string_view("timeout")
};

constexpr std::array env_keys = std::array{
    std::string_view("app_env"),
    std::string_view("log_level")
};

// ============================================================================
// STRUCT WITH CONTAINERS MAP FIELDS
// ============================================================================

struct AppConfig {
    std::string name;
    meta::ContainersMap<std::string, std::string, config_keys> config;
    meta::ContainersMap<std::string, std::string, env_keys> environment;
    
    static constexpr auto fields = std::tuple{
        meta::Field<&AppConfig::name>("name", "App name", meta::RequiredField),
        meta::Field<&AppConfig::config>("config", "Config settings", meta::RequiredField),
        meta::Field<&AppConfig::environment>("environment", "Environment vars", meta::RequiredField)
    };
};

int main() {
    std::cout << "=== ContainersMap with fromYaml ===\n\n";

    // ========================================
    // Example 1: Valid YAML
    // ========================================
    std::cout << "--- Example 1: Valid ---\n";

    YAML::Node yaml1 = YAML::Load(R"(
        name: MyApp
        config:
            hostname: localhost
            port: "8080"
            timeout: "30"
        environment:
            app_env: production
            log_level: info
    )");

    auto config1 = meta::fromYaml<AppConfig>(yaml1);

    if (config1) {
        std::cout << "✓ Parsed successfully\n";
        std::cout << "  Name: " << config1->name << "\n";
        std::cout << "  Config:\n";
        for (const auto& [k, v] : config1->config) {
            std::cout << "    " << k << " = " << v << "\n";
        }
        std::cout << "  Environment:\n";
        for (const auto& [k, v] : config1->environment) {
            std::cout << "    " << k << " = " << v << "\n";
        }
    }

    // ========================================
    // Example 2: Invalid config key
    // ========================================
    std::cout << "\n--- Example 2: Invalid Config Key ---\n";

    YAML::Node yaml2 = YAML::Load(R"(
        name: MyApp
        config:
            hostname: localhost
            invalid_key: value
        environment:
            app_env: production
    )");

    auto [config2, result2] = meta::fromYamlWithValidation<AppConfig>(yaml2);
    if (!config2) {
        std::cout << "✗ Parse failed (expected):\n";
        for (const auto& [field, error] : result2.errors) {
            std::cout << "  " << field << ": " << error << "\n";
        }
    } else {
        std::cout << "✓ Parsed (unexpected!)\n";
    }

    std::cout << "\n=== Done ===\n";
    return 0;
}
