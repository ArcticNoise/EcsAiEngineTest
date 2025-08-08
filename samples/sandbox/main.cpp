#include "engine/render/gl/shader.hpp"
#include "engine/runtime/runtime.hpp"
#include <glad/gl.h>

int main() {
  engine::Runtime app;
  if (!app.init()) {
    return -1;
  }

  const char *vsSrc = R"(
    #version 330 core
    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec3 aColor;
    out vec3 vColor;
    void main(){
      vColor = aColor;
      gl_Position = vec4(aPos,1.0);
    }
  )";
  const char *fsSrc = R"(
    #version 330 core
    in vec3 vColor;
    out vec4 FragColor;
    void main(){
      FragColor = vec4(vColor,1.0);
    }
  )";

  engine::render::gl::Shader vs(GL_VERTEX_SHADER);
  engine::render::gl::Shader fs(GL_FRAGMENT_SHADER);
  if (!vs.compile(vsSrc) || !fs.compile(fsSrc)) {
    return -1;
  }

  engine::render::gl::Program program;
  program.attach(vs);
  program.attach(fs);
  if (!program.link()) {
    return -1;
  }

  float vertices[] = {
      // positions       // colors
      -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, -0.5f, 0.0f,
      0.0f,  1.0f,  0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f,  1.0f,
  };
  GLuint vao = 0, vbo = 0;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  while (app.tick()) {
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    program.use();
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glfwSwapBuffers(app.window());
  }

  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  return 0;
}
