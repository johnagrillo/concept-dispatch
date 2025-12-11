#include "meta.h"
#include <iostream>

// Simple struct with fields
struct Person {
  std::string name;
  int age;
  bool active;

  // Define fields as tuple
  static constexpr auto fields = std::tuple{
    meta::Field<&Person::name>("name", "Person's name", meta::RequiredField),
    meta::Field<&Person::age>("age", "Person's age", meta::RequiredField),
    meta::Field<&Person::active>("active", "Is person active", meta::RequiredField)
  };
};

int main() {
  // Create YAML
  YAML::Node yaml = YAML::Load(R"(
    name: Alice
    age: 30
    active: true
  )");

  // Parse from YAML
  auto person = meta::fromYaml<Person>(yaml);
  if (person) {
    std::cout << "=== toString ===\n" << meta::toString(*person) << "\n";
    std::cout << "=== toJson ===\n" << meta::toJson(*person) << "\n";
    
    auto yamlMap = meta::toYamlMap(*person);
    std::cout << "=== toYamlMap ===\n";
    for (const auto& [key, val] : yamlMap) {
      std::cout << key << ": " << val << "\n";
    }
  }

  return 0;
}
