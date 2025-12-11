#include "containers2.h"
#include <iostream>
#include <memory>

int main() {
    std::cout << "=== Constrained Containers with Whitelists ===\n\n";

    // ========================================
    // Vector: Positive integers
    // ========================================
    std::cout << "--- Vector: Positive Integers ---\n";

    using PositiveIntVector = meta::ConstrainedVector<int, meta::PositiveConstraint>;
    PositiveIntVector scores;

    try {
        scores.push_back(95);
        scores.push_back(87);
        scores.push_back(92);
        std::cout << "✓ Valid scores inserted\n";
        for (size_t i = 0; i < scores.size(); i++) {
            std::cout << "  Score " << i << ": " << scores[i] << "\n";
        }
    } catch (const std::exception& e) {
        std::cout << "✗ " << e.what() << "\n";
    }

    // ========================================
    // Map: String keys (non-empty), positive ints
    // ========================================
    std::cout << "\n--- Map: Non-Empty Keys, Positive Values ---\n";

    using ConfigMap = meta::ConstrainedMap<
        std::string,
        int,
        meta::NonEmptyStringConstraint,
        meta::PositiveConstraint
    >;

    ConfigMap config;

    try {
        config.insert("timeout", 30);
        config.insert("retries", 3);
        std::cout << "✓ Valid config entries inserted\n";
        for (const auto& [key, value] : config) {
            std::cout << "  " << key << ": " << value << "\n";
        }
    } catch (const std::exception& e) {
        std::cout << "✗ " << e.what() << "\n";
    }

    // ========================================
    // Map: Only certain keys allowed (whitelist)
    // ========================================
    std::cout << "\n--- Map: Only Certain Keys (Whitelist) ---\n";

    std::cout << "Allowed keys: host, port, timeout\n";

    // Simulate validation
    for (const std::string& key : {"host", "port", "timeout", "invalid"}) {
        meta::StringKeyWhitelist kw(std::vector<std::string>{"host", "port", "timeout"});
        if (kw.validate(key)) {
            std::cout << "  ✓ '" << key << "' is allowed\n";
        } else {
            std::cout << "  ✗ '" << key << "' is NOT allowed: " << kw.error(key) << "\n";
        }
    }

    // ========================================
    // Map: Only certain values allowed (whitelist)
    // ========================================
    std::cout << "\n--- Map: Only Certain Values (Whitelist) ---\n";

    meta::StringValueWhitelist envWhitelist(
        std::vector<std::string>{"development", "staging", "production"}
    );

    std::cout << "Allowed values: development, staging, production\n";

    for (const std::string& val : {"development", "staging", "production", "invalid"}) {
        if (envWhitelist.validate(val)) {
            std::cout << "  ✓ '" << val << "' is allowed\n";
        } else {
            std::cout << "  ✗ '" << val << "' is NOT allowed: " << envWhitelist.error(val) << "\n";
        }
    }

    // ========================================
    // Map: Int values from whitelist
    // ========================================
    std::cout << "\n--- Map: Only Certain Int Values (Whitelist) ---\n";

    meta::IntValueWhitelist portWhitelist(std::vector<int>{8080, 8081, 8082, 3000, 5000});

    std::cout << "Allowed ports: 8080, 8081, 8082, 3000, 5000\n";

    for (int port : {8080, 8081, 9000, 3000, 9999}) {
        if (portWhitelist.validate(port)) {
            std::cout << "  ✓ Port " << port << " is allowed\n";
        } else {
            std::cout << "  ✗ Port " << port << " is NOT allowed: " << portWhitelist.error(port) << "\n";
        }
    }

    // ========================================
    // Map: Both keys AND values constrained
    // ========================================
    std::cout << "\n--- Map: Keys AND Values Both Constrained ---\n";

    using ConstrainedBothMap = meta::ConstrainedMap<
        std::string,
        std::string,
        meta::NonEmptyStringConstraint,
        meta::NonEmptyStringConstraint
    >;

    ConstrainedBothMap mapping;

    try {
        mapping.insert("name", "Alice");
        mapping.insert("city", "Boston");
        std::cout << "✓ Both key and value constraints satisfied\n";
        for (const auto& [key, value] : mapping) {
            std::cout << "  " << key << " -> " << value << "\n";
        }
    } catch (const std::exception& e) {
        std::cout << "✗ " << e.what() << "\n";
    }

    try {
        mapping.insert("", "value");
        std::cout << "✓ Inserted\n";
    } catch (const std::exception& e) {
        std::cout << "✗ Empty key rejected: " << e.what() << "\n";
    }

    try {
        mapping.insert("key", "");
        std::cout << "✓ Inserted\n";
    } catch (const std::exception& e) {
        std::cout << "✗ Empty value rejected: " << e.what() << "\n";
    }

    std::cout << "\n=== Done ===\n";
    return 0;
}
