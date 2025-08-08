#include "engine/render/gl/shader.hpp"

namespace engine::render::gl {

Shader::Shader(GLenum type) : id_(glCreateShader(type)) {}

Shader::~Shader() noexcept {
  if (id_ != 0)
    glDeleteShader(id_);
}

Shader::Shader(Shader &&other) noexcept : id_(other.id_) { other.id_ = 0; }

Shader &Shader::operator=(Shader &&other) noexcept {
  if (this != &other) {
    if (id_ != 0)
      glDeleteShader(id_);
    id_ = other.id_;
    other.id_ = 0;
  }
  return *this;
}

bool Shader::compile(std::string_view source) noexcept {
  const char *src = source.data();
  GLint len = static_cast<GLint>(source.size());
  glShaderSource(id_, 1, &src, &len);
  glCompileShader(id_);
  GLint status = 0;
  glGetShaderiv(id_, GL_COMPILE_STATUS, &status);
  return status == GL_TRUE;
}

Program::Program() : id_(glCreateProgram()) {}

Program::~Program() noexcept {
  if (id_ != 0)
    glDeleteProgram(id_);
}

Program::Program(Program &&other) noexcept : id_(other.id_) { other.id_ = 0; }

Program &Program::operator=(Program &&other) noexcept {
  if (this != &other) {
    if (id_ != 0)
      glDeleteProgram(id_);
    id_ = other.id_;
    other.id_ = 0;
  }
  return *this;
}

void Program::attach(const Shader &shader) noexcept { glAttachShader(id_, shader.id()); }

bool Program::link() noexcept {
  glLinkProgram(id_);
  GLint status = 0;
  glGetProgramiv(id_, GL_LINK_STATUS, &status);
  return status == GL_TRUE;
}

void Program::use() const noexcept { glUseProgram(id_); }

} // namespace engine::render::gl
