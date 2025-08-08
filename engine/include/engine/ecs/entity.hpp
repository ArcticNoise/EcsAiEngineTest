#pragma once

#include <cstdint>

namespace ecs {
using EntityHandle = std::uint32_t;

struct Entity {
  EntityHandle id{0};
  std::uint32_t gen{0};
  friend constexpr bool operator==(const Entity &, const Entity &) = default;
};
} // namespace ecs
