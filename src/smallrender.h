#ifndef SMALL_RENDERER_H
#define SMALL_RENDERER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include<vector>
#include<string>

class SmallRenderer{
 private:
  void initWindow();
  
  GLFWwindow* m_window;
  const int m_width;
  const int m_height;

  GLuint m_VertexArrayID;
  GLuint m_shaderProgram;
  GLuint m_vao;          // Vertex Array Object
  size_t m_numIndices;   // Number of indices for drawing

 public:
 SmallRenderer(const int width, const int height) : m_width{width}, m_height{height} {}
  ~SmallRenderer(){cleanUp();};
  void init(std::string& model);
  void run();
  void loadModel(std::string& path);
  void render();
  void initShader();
  void cleanUp();
};

#endif
