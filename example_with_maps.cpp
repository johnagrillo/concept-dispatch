#include "meta.h"
#include <iostream>

struct Config {
  std::string title;
  std::map<std::string, std::string> settings;
  std::vector<std::string> tags;
  double timeout;

  static constexpr auto fields = std::tuple{
    meta::Field<&Config::title>("title", "Config title", meta::RequiredField),
    meta::Field<&Config::settings>("settings", "Key-value settings", meta::RequiredField),
    meta::Field<&Config::tags>("tags", "Tag list", meta::RequiredField),
    meta::Field<&Config::timeout>("timeout", "Timeout in seconds", meta::RequiredField)
  };
};

int main() {
  YAML::Node yaml = YAML::Load(R"(
    title: MyApp
    settings:
      color: blue
      mode: debug
    tags:
      - important
      - production
    timeout: 30.5
  )");

  auto config = meta::fromYaml<Config>(yaml);
  if (config) {
    std::cout << meta::toString(*config);
    std::cout << "\n=== JSON ===\n" << meta::toJson(*config);
  }

  return 0;
}
