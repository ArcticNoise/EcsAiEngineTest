#pragma once

#include <cstddef>
#include <glad/gl.h>

namespace engine::render::gl {

class Buffer {
public:
  explicit Buffer(GLenum target);
  ~Buffer() noexcept;
  Buffer(const Buffer &) = delete;
  Buffer &operator=(const Buffer &) = delete;
  Buffer(Buffer &&other) noexcept;
  Buffer &operator=(Buffer &&other) noexcept;

  void bind() const noexcept;
  void setData(const void *data, std::size_t size, GLenum usage) const noexcept;
  [[nodiscard]] GLuint id() const noexcept { return id_; }

private:
  GLuint id_{};
  GLenum target_{};
};

} // namespace engine::render::gl
