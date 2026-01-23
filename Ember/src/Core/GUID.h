#pragma once

#include <cstdint>
#include <random>

#include <spdlog/fmt/fmt.h>
#include <yaml-cpp/yaml.h>

namespace Ember {
class GUID {
public:
  inline GUID() {
    // Random uint64_t from 1 to max
    // 0 is for none guid
    std::uniform_int_distribution<unsigned long long> dis(
        std::numeric_limits<std::uint64_t>::min() + 1,
        std::numeric_limits<std::uint64_t>::max());
    std::random_device rd;
    std::mt19937 gen(rd());
    mID = dis(gen);
  }
  inline GUID(uint64_t value) {
    mID = value;
  }
  static GUID NONE() {
    GUID guid;
    guid.mID = 0;
    return guid;
  }

  inline uint64_t GetID() const { return mID; }

  friend bool operator==(const GUID &a, const GUID &b) {
    return a.mID == b.mID;
  }
  friend bool operator!=(const GUID &a, const GUID &b) {
    return a.mID != b.mID;
  }

private:
  uint64_t mID;
};

} // namespace Ember

// YAML serialization
namespace YAML {
template <> struct convert<Ember::GUID> {
  static Node encode(const Ember::GUID &guid) {
    Node node(guid.GetID());
    return node;
  }

  static bool decode(const Node &node, Ember::GUID &guid) {
    guid = node.as<uint64_t>();
    return true;
  }
};
} // namespace YAML

// Hash
template <> struct std::hash<Ember::GUID> {
  std::size_t operator()(const Ember::GUID &guid) const noexcept {
    return std::hash<uint64_t>{}(guid.GetID());
  }
};
// Formater
template <> struct fmt::formatter<Ember::GUID> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const Ember::GUID &guid, FormatContext &ctx) const {
    return fmt::format_to(ctx.out(), "{}", guid.GetID());
  }
};
