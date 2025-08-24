#pragma once

#include <glad/gl.h>

namespace engine::render::gl {

class VertexArray {
public:
  VertexArray();
  ~VertexArray() noexcept;
  VertexArray(const VertexArray &) = delete;
  VertexArray &operator=(const VertexArray &) = delete;
  VertexArray(VertexArray &&other) noexcept;
  VertexArray &operator=(VertexArray &&other) noexcept;

  void bind() const noexcept;
  [[nodiscard]] GLuint id() const noexcept { return id_; }

private:
  GLuint id_{};
};

} // namespace engine::render::gl
