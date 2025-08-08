#include "engine/render/gl/buffer.hpp"

namespace engine::render::gl {

Buffer::Buffer(GLenum target) : target_(target) { glGenBuffers(1, &id_); }

Buffer::~Buffer() noexcept {
  if (id_ != 0)
    glDeleteBuffers(1, &id_);
}

Buffer::Buffer(Buffer &&other) noexcept : id_(other.id_), target_(other.target_) { other.id_ = 0; }

Buffer &Buffer::operator=(Buffer &&other) noexcept {
  if (this != &other) {
    if (id_ != 0)
      glDeleteBuffers(1, &id_);
    id_ = other.id_;
    target_ = other.target_;
    other.id_ = 0;
  }
  return *this;
}

void Buffer::bind() const noexcept { glBindBuffer(target_, id_); }

void Buffer::setData(const void *data, std::size_t size, GLenum usage) const noexcept {
  glBufferData(target_, static_cast<GLsizeiptr>(size), data, usage);
}

} // namespace engine::render::gl
