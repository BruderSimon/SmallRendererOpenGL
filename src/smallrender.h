#ifndef SMALL_RENDERER_H
#define SMALL_RENDERER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include "common/tiny_obj_loader.h"
#include "sceneobject.h"
#include "camera.h"

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

  Camera m_camera;

  bool m_mouseMiddlePressed; // Track if the middle mouse button is pressed
  glm::vec2 m_lastMousePos; // Store the last mouse position for rotation
  
  std::vector<SceneObject> m_sceneObjects;
  
public:
 SmallRenderer(const int width, const int height) :
   m_width{width}, m_height{height}, m_mouseMiddlePressed{false} {}
  ~SmallRenderer(){cleanUp();};
  void init(std::string &model, std::string mtl);
  void loadScene(std::string& path);
  void run();
  void render();
  void initShader();
  void cleanUp();

  static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
  static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
};

#endif
