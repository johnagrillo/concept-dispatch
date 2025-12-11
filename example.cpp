#include <map>
#include <string>
#include <vector>

#include "meta.h"

namespace examples
{

// Example 1: Simple User struct
struct User
{
    std::string username;
    std::string email;
    int age;
    bool active;

    static constexpr auto fields =
        std::tuple{meta::Field<&User::username>("username",
                                                "User login name",
                                                meta::RequiredField,
                                                meta::StringConstraint{1, 32}),
                   meta::Field<&User::email>("email",
                                             "User email address",
                                             meta::RequiredField,
                                             meta::StringConstraint{5, 255}),
                   meta::Field<&User::age>("age",
                                           "User age in years",
                                           meta::RequiredField,
                                           meta::IntConstraint{0, 150}),
                   meta::Field<&User::active>("active",
                                              "Is user account active",
                                              meta::RequiredField,
                                              meta::BoolConstraint{})};
};

// Example 2: Config with enums
enum class LogLevel
{
    Debug,
    Info,
    Warning,
    Error,
    Fatal
};

struct AppConfig
{
    std::string app_name;
    LogLevel log_level;
    int max_connections;
    double timeout_seconds;
    std::map<std::string, std::string> env_vars;

    static constexpr auto fields = std::tuple{
        meta::Field<&AppConfig::app_name>("app_name",
                                          "Application name",
                                          meta::RequiredField,
                                          meta::StringConstraint{1, 64}),
        meta::Field<&AppConfig::log_level>("log_level",
                                           "Logging level",
                                           meta::RequiredField,
                                           meta::EnumConstraint<LogLevel>{}),
        meta::Field<&AppConfig::max_connections>("max_connections",
                                                 "Maximum concurrent connections",
                                                 meta::RequiredField,
                                                 meta::IntConstraint{1, 10000}),
        meta::Field<&AppConfig::timeout_seconds>("timeout_seconds",
                                                 "Request timeout in seconds",
                                                 meta::RequiredField,
                                                 meta::FloatConstraint{0.1, 3600.0}),
        /*
        meta::Field<&AppConfig::env_vars>(
          "env_vars",
          "Environment variables",
          meta::OptionalField,
          {0, 100}
        )
        */
    };
};

// Example 3: Nested with arrays
struct DatabaseConfig
{
    std::string host;
    int port;
    std::string database;
    std::vector<std::string> backup_hosts;

    static constexpr auto fields =
        std::tuple{meta::Field<&DatabaseConfig::host>("host",
                                                      "Database host",
                                                      meta::RequiredField,
                                                      meta::StringConstraint{1, 255}),
                   meta::Field<&DatabaseConfig::port>("port",
                                                      "Database port",
                                                      meta::RequiredField,
                                                      meta::IntConstraint{1, 65535}),
                   meta::Field<&DatabaseConfig::database>("database",
                                                          "Database name",
                                                          meta::RequiredField,
                                                          meta::StringConstraint{1, 64}),
                   meta::Field<&DatabaseConfig::backup_hosts>("backup_hosts",
                                                              "Backup database hosts",
                                                              meta::OptionalField,
                                                              meta::ArrayConstraint{0, 5})};
};

// Example 4: Complex nested struct
struct ServiceConfig
{
    std::string service_name;
    DatabaseConfig db;
    AppConfig app;
    std::vector<std::string> features;

    static constexpr auto fields =
        std::tuple{meta::Field<&ServiceConfig::service_name>("service_name",
                                                             "Service identifier",
                                                             meta::RequiredField,
                                                             meta::StringConstraint{1, 128}),
                   meta::Field<&ServiceConfig::db>("database",
                                                   "Database configuration",
                                                   meta::RequiredField,
                                                   "DatabaseConfig"),
                   meta::Field<&ServiceConfig::app>("application",
                                                    "Application configuration",
                                                    meta::RequiredField,
                                                    "AppConfig"),
                   meta::Field<&ServiceConfig::features>("features",
                                                         "Enabled features",
                                                         meta::OptionalField,
                                                         meta::ArrayConstraint{0, 50})};
};




} // namespace examples

#include <iostream>

int main()
{
    std::cout << "=== Simple YAML Parsing Tests ===\n\n";

    // Test 1: Valid User
    {
        std::cout << "Test 1: Valid User\n";
        YAML::Node yaml = YAML::Load(R"(
      username: alice
      email: alice@example.com
      age: 30
      active: true
    )");

        auto [user, result] = meta::fromYamlWithValidation<examples::User>(yaml);
        if (result.valid)
        {
            std::cout << "✓ Parsed successfully\n";
            std::cout << meta::toString(*user) << "\n";
        }
        else
        {
            std::cout << "✗ Failed\n";
        }
    }

    // Test 2: Invalid - age out of range
    {
        std::cout << "\nTest 2: Age out of range (should fail)\n";
        YAML::Node yaml = YAML::Load(R"(
      username: bob
      email: bob@example.com
      age: 200
      active: true
    )");

        auto [user, result] = meta::fromYamlWithValidation<examples::User>(yaml);
        if (!result.valid)
        {
            std::cout << "✓ Correctly rejected\n";
            for (const auto& [field, error] : result.errors)
            {
                std::cout << "  " << field << ": " << error << "\n";
            }
        }
    }

    // Test 3: Valid AppConfig
    {
        std::cout << "\nTest 3: Valid AppConfig\n";
        YAML::Node yaml = YAML::Load(R"(
      app_name: MyApp
      log_level: Info
      max_connections: 100
      timeout_seconds: 30.5
    )");

        auto [cfg, result] = meta::fromYamlWithValidation<examples::AppConfig>(yaml);
        if (result.valid)
        {
            std::cout << "✓ Parsed successfully\n";
            std::cout << meta::toString(*cfg) << "\n";
        }
    }

    // Test 4: Invalid - max_connections too high
    {
        std::cout << "\nTest 4: max_connections too high (should fail)\n";
        YAML::Node yaml = YAML::Load(R"(
      app_name: MyApp
      log_level: Debug
      max_connections: 50000
      timeout_seconds: 30.0
    )");

        auto [cfg, result] = meta::fromYamlWithValidation<examples::AppConfig>(yaml);
        if (!result.valid)
        {
            std::cout << "✓ Correctly rejected\n";
            for (const auto& [field, error] : result.errors)
            {
                std::cout << "  " << field << ": " << error << "\n";
            }
        }
    }

    // Test 5: DatabaseConfig with arrays
    {
        std::cout << "\nTest 5: Valid DatabaseConfig\n";
        YAML::Node yaml = YAML::Load(R"(
      host: localhost
      port: 5432
      database: mydb
      backup_hosts:
        - backup1.example.com
        - backup2.example.com
    )");

        auto [db, result] = meta::fromYamlWithValidation<examples::DatabaseConfig>(yaml);
        if (result.valid)
        {
            std::cout << "✓ Parsed successfully\n";
            // std::cout << meta::toString(*db) << "\n";
        }
    }

    std::cout << "\n=== All Tests Complete ===\n";
    return 0;
}
