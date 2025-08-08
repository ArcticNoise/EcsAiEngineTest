#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace engine {
class Runtime {
public:
  Runtime() = default;
  ~Runtime() noexcept;
  [[nodiscard]] bool init() noexcept;
  [[nodiscard]] bool tick() noexcept;
  [[nodiscard]] GLFWwindow *window() noexcept { return window_; }

private:
  GLFWwindow *window_ = nullptr;
};
} // namespace engine
