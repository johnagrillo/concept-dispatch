#include "meta.h"
#include "vector.h"
#include <iostream>

// ============================================================================
// COMPILE-TIME WHITELISTS FOR VECTORS
// ============================================================================

constexpr std::array allowed_envs = std::array{
    std::string_view("dev"),
    std::string_view("staging"),
    std::string_view("prod")
};

constexpr std::array allowed_levels = std::array{
    std::string_view("debug"),
    std::string_view("info"),
    std::string_view("warn"),
    std::string_view("error")
};

// ============================================================================
// STRUCT WITH WHITELISTED VECTORS
// ============================================================================

struct AppConfig {
    std::string name;
    meta::WhitelistVector<std::string, allowed_envs> environments;
    meta::WhitelistVector<std::string, allowed_levels> log_levels;
    
    static constexpr auto fields = std::tuple{
        meta::Field<&AppConfig::name>("name", "App name", meta::RequiredField),
        meta::Field<&AppConfig::environments>("environments", "Allowed environments", meta::RequiredField),
        meta::Field<&AppConfig::log_levels>("log_levels", "Allowed log levels", meta::RequiredField)
    };
};

int main() {
    std::cout << "=== WhitelistVector with fromYaml ===\n\n";

    // ========================================
    // Example 1: Valid vectors
    // ========================================
    std::cout << "--- Example 1: Valid ---\n";

    YAML::Node yaml1 = YAML::Load(R"(
        name: MyApp
        environments:
            - dev
            - staging
            - prod
        log_levels:
            - debug
            - info
            - error
    )");

    auto [config1, result1] = meta::fromYamlWithValidation<AppConfig>(yaml1);

    if (config1) {
        std::cout << "✓ Parsed successfully\n";
        std::cout << "  Name: " << config1->name << "\n";
        std::cout << "  Environments: ";
        for (size_t i = 0; i < config1->environments.size(); i++) {
            if (i > 0) std::cout << ", ";
            std::cout << config1->environments[i];
        }
        std::cout << "\n";
        std::cout << "  Log Levels: ";
        for (size_t i = 0; i < config1->log_levels.size(); i++) {
            if (i > 0) std::cout << ", ";
            std::cout << config1->log_levels[i];
        }
        std::cout << "\n";
    }

    // ========================================
    // Example 2: Invalid environment value
    // ========================================
    std::cout << "\n--- Example 2: Invalid Environment ---\n";

    YAML::Node yaml2 = YAML::Load(R"(
        name: MyApp
        environments:
            - dev
            - invalid_env
        log_levels:
            - debug
    )");

    auto [config2, result2] = meta::fromYamlWithValidation<AppConfig>(yaml2);

    if (!config2) {
        std::cout << "✗ Parse failed (expected):\n";
        for (const auto& [field, error] : result2.errors) {
            std::cout << "  " << field << ": " << error << "\n";
        }
    }

    // ========================================
    // Example 3: Invalid log level
    // ========================================
    std::cout << "\n--- Example 3: Invalid Log Level ---\n";

    YAML::Node yaml3 = YAML::Load(R"(
        name: MyApp
        environments:
            - dev
            - prod
        log_levels:
            - debug
            - trace
    )");

    auto [config3, result3] = meta::fromYamlWithValidation<AppConfig>(yaml3);

    if (!config3) {
        std::cout << "✗ Parse failed (expected):\n";
        for (const auto& [field, error] : result3.errors) {
            std::cout << "  " << field << ": " << error << "\n";
        }
    }

    std::cout << "\n=== Done ===\n";
    return 0;
}
