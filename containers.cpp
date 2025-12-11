#include "containers.h"
#include <iostream>

int main() {
    std::cout << "=== Constrained Containers ===\n\n";

    // ========================================
    // Map: string keys, positive int values
    // ========================================
    std::cout << "--- Map: String Keys, Positive Int Values ---\n";

    using StringKeyMap = meta::ConstrainedMap<
        std::string,
        int,
        meta::NonEmptyStringConstraint,
        meta::PositiveConstraint
    >;

    StringKeyMap config;

    try {
        config.insert("timeout", 30);
        config.insert("retries", 3);
        config.insert("max_connections", 100);
        std::cout << "✓ Inserted valid entries\n";

        for (const auto& [key, value] : config) {
            std::cout << "  " << key << ": " << value << "\n";
        }
    } catch (const std::exception& e) {
        std::cout << "✗ Error: " << e.what() << "\n";
    }

    // ========================================
    // Map: Invalid key (empty string)
    // ========================================
    std::cout << "\n--- Map: Invalid Key (Empty String) ---\n";

    try {
        config.insert("", 42);
        std::cout << "✓ Inserted\n";
    } catch (const std::exception& e) {
        std::cout << "✗ " << e.what() << "\n";
    }

    // ========================================
    // Map: Invalid value (negative)
    // ========================================
    std::cout << "\n--- Map: Invalid Value (Negative) ---\n";

    try {
        config.insert("invalid_timeout", -1);
        std::cout << "✓ Inserted\n";
    } catch (const std::exception& e) {
        std::cout << "✗ " << e.what() << "\n";
    }

    // ========================================
    // Vector: Positive integers only
    // ========================================
    std::cout << "\n--- Vector: Positive Integers ---\n";

    using PositiveIntVector = meta::ConstrainedVector<
        int,
        meta::PositiveConstraint
    >;

    PositiveIntVector scores;

    try {
        scores.push_back(95);
        scores.push_back(87);
        scores.push_back(92);
        std::cout << "✓ Inserted valid scores\n";

        for (size_t i = 0; i < scores.size(); i++) {
            std::cout << "  Score " << i << ": " << scores[i] << "\n";
        }
    } catch (const std::exception& e) {
        std::cout << "✗ Error: " << e.what() << "\n";
    }

    // ========================================
    // Vector: Invalid element (zero)
    // ========================================
    std::cout << "\n--- Vector: Invalid Element (Zero) ---\n";

    try {
        scores.push_back(0);
        std::cout << "✓ Inserted\n";
    } catch (const std::exception& e) {
        std::cout << "✗ " << e.what() << "\n";
    }

    // ========================================
    // Vector: Invalid element (negative)
    // ========================================
    std::cout << "\n--- Vector: Invalid Element (Negative) ---\n";

    try {
        scores.push_back(-5);
        std::cout << "✓ Inserted\n";
    } catch (const std::exception& e) {
        std::cout << "✗ " << e.what() << "\n";
    }

    // ========================================
    // Vector: Non-empty strings
    // ========================================
    std::cout << "\n--- Vector: Non-Empty Strings ---\n";

    using NonEmptyStringVector = meta::ConstrainedVector<
        std::string,
        meta::NonEmptyStringConstraint
    >;

    NonEmptyStringVector tags;

    try {
        tags.push_back("important");
        tags.push_back("urgent");
        tags.push_back("follow-up");
        std::cout << "✓ Inserted valid tags\n";

        for (size_t i = 0; i < tags.size(); i++) {
            std::cout << "  Tag " << i << ": " << tags[i] << "\n";
        }
    } catch (const std::exception& e) {
        std::cout << "✗ Error: " << e.what() << "\n";
    }

    // ========================================
    // Vector: Invalid element (empty string)
    // ========================================
    std::cout << "\n--- Vector: Invalid Element (Empty String) ---\n";

    try {
        tags.push_back("");
        std::cout << "✓ Inserted\n";
    } catch (const std::exception& e) {
        std::cout << "✗ " << e.what() << "\n";
    }

    std::cout << "\n=== Done ===\n";
    return 0;
}

