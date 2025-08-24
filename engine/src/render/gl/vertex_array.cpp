#include "engine/render/gl/vertex_array.hpp"

namespace engine::render::gl {

VertexArray::VertexArray() { glGenVertexArrays(1, &id_); }

VertexArray::~VertexArray() noexcept {
  if (id_ != 0)
    glDeleteVertexArrays(1, &id_);
}

VertexArray::VertexArray(VertexArray &&other) noexcept : id_(other.id_) { other.id_ = 0; }

VertexArray &VertexArray::operator=(VertexArray &&other) noexcept {
  if (this != &other) {
    if (id_ != 0)
      glDeleteVertexArrays(1, &id_);
    id_ = other.id_;
    other.id_ = 0;
  }
  return *this;
}

void VertexArray::bind() const noexcept { glBindVertexArray(id_); }

} // namespace engine::render::gl
