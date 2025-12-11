#include "meta.h"
#include <iostream>

// ============================================================================
// ENUM DEFINITION
// ============================================================================

enum class LogLevel {
  Debug,
  Info,
  Warning,
  Error
};

// ============================================================================
// ENUM MAPPING (defines how to convert to/from string)
// ============================================================================

template <>
struct meta::EnumMapping<LogLevel> {
  struct Type {
    static std::optional<LogLevel> fromString(const std::string& str) {
      if (str == "DEBUG") return LogLevel::Debug;
      if (str == "INFO") return LogLevel::Info;
      if (str == "WARNING") return LogLevel::Warning;
      if (str == "ERROR") return LogLevel::Error;
      return {};
    }

    static std::string toString(LogLevel level) {
      switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARNING";
        case LogLevel::Error: return "ERROR";
      }
      return "UNKNOWN";
    }
  };
};

// ============================================================================
// STRUCT WITH ENUM FIELD
// ============================================================================

struct LogEntry {
  std::string message;
  LogLevel level;
  int line_number;

  static constexpr auto fields = std::tuple{
    meta::Field<&LogEntry::message>("message", "Log message", meta::RequiredField),
    meta::Field<&LogEntry::level>("level", "Log level", meta::RequiredField),
    meta::Field<&LogEntry::line_number>("line_number", "Line number", meta::RequiredField)
  };
};

// ============================================================================
// USAGE
// ============================================================================

int main() {
  YAML::Node yaml = YAML::Load(R"(
    message: Something went wrong
    level: ERROR
    line_number: 42
  )");

  auto entry = meta::fromYaml<LogEntry>(yaml);
  if (entry) {
    std::cout << "=== toString ===\n" << meta::toString(*entry) << "\n";
    std::cout << "=== toJson ===\n" << meta::toJson(*entry);
  }

  return 0;
}
