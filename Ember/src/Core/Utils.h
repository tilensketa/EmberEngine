#pragma once

#include <string>
#include <array>

namespace Ember::Utils {

// Primary template — undefined on purpose
template <typename Enum> struct EnumTraits;

// Generic ToString / FromString functions
template <typename Enum> constexpr const char *ToString(Enum e) {
  return EnumTraits<Enum>::Names[static_cast<size_t>(e)];
}

template <typename Enum> constexpr Enum FromString(const std::string &str) {
  for (size_t i = 0; i < EnumTraits<Enum>::Names.size(); ++i) {
    if (str == EnumTraits<Enum>::Names[i]) return static_cast<Enum>(i);
  }
  return static_cast<Enum>(0); // default first value
}

} // namespace Ember::Utils
