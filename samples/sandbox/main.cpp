#include "engine/runtime/runtime.hpp"
#include <glad/gl.h>

static GLuint createShader(GLenum type, const char *src) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &src, nullptr);
  glCompileShader(shader);
  return shader;
}

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

  GLuint vs = createShader(GL_VERTEX_SHADER, vsSrc);
  GLuint fs = createShader(GL_FRAGMENT_SHADER, fsSrc);
  GLuint program = glCreateProgram();
  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);
  glDeleteShader(vs);
  glDeleteShader(fs);

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
    glUseProgram(program);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glfwSwapBuffers(app.window());
  }

  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteProgram(program);
  return 0;
}
