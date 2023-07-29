#pragma once

#include <cstddef>

class Hashable {
public:
  virtual ~Hashable() = default;

  static constexpr std::size_t Invalid = static_cast<std::size_t>(-1);

  virtual std::size_t hash() const noexcept { return Invalid; }
};
