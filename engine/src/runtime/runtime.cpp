#include "engine/runtime/runtime.hpp"
#include <glad/gl.h>

namespace engine {
Runtime::~Runtime() noexcept {
  if (window_) {
    glfwDestroyWindow(window_);
    glfwTerminate();
  }
}

bool Runtime::init() noexcept {
  if (!glfwInit()) {
    return false;
  }
#ifdef ENGINE_GL_MIN
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#else
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
#endif
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  window_ = glfwCreateWindow(640, 480, "Sandbox", nullptr, nullptr);
  if (!window_) {
    glfwTerminate();
    return false;
  }
  glfwMakeContextCurrent(window_);
  if (gladLoadGL(glfwGetProcAddress) == 0) {
    return false;
  }
  return true;
}

bool Runtime::tick() noexcept {
  if (!window_ || glfwWindowShouldClose(window_)) {
    return false;
  }
  glfwPollEvents();
  return true;
}
} // namespace engine
