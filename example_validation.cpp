#include "meta.h"
#include <iostream>

struct User {
  std::string username;
  std::string email;
  int score;

  static constexpr auto fields = std::tuple{
    meta::Field<&User::username>("username", "Username", meta::RequiredField),
    meta::Field<&User::email>("email", "Email address", meta::RequiredField),
    meta::Field<&User::score>("score", "User score", meta::RequiredField)
  };
};

int main() {
  // Valid YAML
  YAML::Node valid = YAML::Load(R"(
    username: john_doe
    email: john@example.com
    score: 100
  )");

  auto [user, result] = meta::fromYamlWithValidation<User>(valid);
  if (user) {
    std::cout << "✓ Valid!\n" << meta::toString(*user);
  } else {
    std::cout << "✗ Invalid:\n";
    for (const auto& [field, msg] : result.errors) {
      std::cout << "  " << field << ": " << msg << "\n";
    }
  }

  // Missing required field
  YAML::Node invalid = YAML::Load(R"(
    username: jane_doe
  )");

  auto [user2, result2] = meta::fromYamlWithValidation<User>(invalid);
  if (!user2) {
    std::cout << "\n✗ Missing fields:\n";
    for (const auto& [field, msg] : result2.errors) {
      std::cout << "  " << field << ": " << msg << "\n";
    }
  }

  return 0;
}
