#ifndef SMALL_RENDERER_H
#define SMALL_RENDERER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include "common/tiny_obj_loader.h"

#include<vector>
#include<string>

class SmallRenderer{
 private:
  void initWindow();
  void checkGLError(const char* operation);
  
  GLFWwindow* m_window;
  const int m_width;
  const int m_height;

  GLuint m_shaderProgram;
  GLuint m_vao;          // Vertex Array Object
  size_t m_numIndices;   // Number of indices for drawing
  GLuint m_vertexBuffer;
  GLuint m_normalBuffer;
  GLuint m_elementBuffer;
  GLuint m_textureID;
  GLuint m_uvBuffer;

  glm::vec2 m_lastMousePos;
  glm::mat4 m_modelRotation;
  bool m_mousePressed;
  std::vector<tinyobj::material_t> m_materials;
  std::map<std::string, GLuint> m_textures;
  
public:
 SmallRenderer(const int width, const int height) :
   m_width{width}, m_height{height},
   m_mousePressed{false}, m_modelRotation{glm::mat4(1.0f)} {}
  ~SmallRenderer(){cleanUp();};
  void init(std::string& model, std::string mtl);
  void run();
  void loadModel(std::string& path, std::string mtlPath);
  void loadTextureFromFile(const std::string& filename, GLuint textureID);
  void render();
  void initShader();
  void cleanUp();

  static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
  static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
};

#endif
