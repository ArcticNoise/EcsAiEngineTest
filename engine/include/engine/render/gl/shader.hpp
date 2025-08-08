#pragma once

#include <glad/gl.h>
#include <string_view>

namespace engine::render::gl {

class Shader {
public:
  explicit Shader(GLenum type);
  ~Shader() noexcept;
  Shader(const Shader &) = delete;
  Shader &operator=(const Shader &) = delete;
  Shader(Shader &&other) noexcept;
  Shader &operator=(Shader &&other) noexcept;

  bool compile(std::string_view source) noexcept;
  [[nodiscard]] GLuint id() const noexcept { return id_; }

private:
  GLuint id_{};
};

class Program {
public:
  Program();
  ~Program() noexcept;
  Program(const Program &) = delete;
  Program &operator=(const Program &) = delete;
  Program(Program &&other) noexcept;
  Program &operator=(Program &&other) noexcept;

  void attach(const Shader &shader) noexcept;
  bool link() noexcept;
  void use() const noexcept;
  [[nodiscard]] GLuint id() const noexcept { return id_; }

private:
  GLuint id_{};
};

} // namespace engine::render::gl
