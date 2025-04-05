#include "smallrender.h"

#include "common/shader.hpp"
#include "glfw3.h"
#include <glm/gtc/matrix_transform.hpp>

#include <cstddef>
#include <cstring>
#include <iostream>
#include <stdexcept>

void SmallRenderer::init(std::string& model, std::string mtl){
  initWindow();
  loadScene(model);
  initShader();
  
}

void SmallRenderer::initWindow(){
  glewExperimental = true;
  if(!glfwInit())
    throw std::runtime_error("Failed to initialize GLFW");
 
  glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); 
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

  m_window = glfwCreateWindow( m_width, m_height, "Small Renderer", NULL, NULL);
  if(m_window == NULL){
    glfwTerminate();
    throw std::runtime_error("Failed to open Window");
  }
  glfwMakeContextCurrent(m_window); // Iniftialize GLEW
  if(glewInit() != GLEW_OK) {
    glfwTerminate();
    throw std::runtime_error("Failed to initialize GLEW");
  }

  // Ensure we can capture keys
  glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_TRUE);
  glClearColor(0.0f, 0.1f, 0.3f, 0.0f);

  // Set mouse callbacks
  glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
  glfwSetCursorPosCallback(m_window, cursorPosCallback);
  glfwSetWindowUserPointer(m_window, this);

  // Window Resize callback
  glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);
}
void SmallRenderer::loadScene(std::string &path) {
  std::string mtl = "./";
  SceneObject object;
  object.loadObject(path, mtl);
  m_sceneObjects.emplace_back(object);
}
void SmallRenderer::run(){
  double lastTime = glfwGetTime();
  while(glfwGetKey(m_window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
	glfwWindowShouldClose(m_window) == 0 ){
    // Calculate delta time
    double currentTime = glfwGetTime();
    float deltaTime = static_cast<float>(currentTime - lastTime);
    lastTime = currentTime;

    // Handle keyboard input for camera movement
    if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
      m_camera.processKeyboard(GLFW_KEY_W, deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
      m_camera.processKeyboard(GLFW_KEY_S, deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
      m_camera.processKeyboard(GLFW_KEY_A, deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
      m_camera.processKeyboard(GLFW_KEY_D, deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS)
      m_camera.processKeyboard(GLFW_KEY_SPACE, deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
      m_camera.processKeyboard(GLFW_KEY_LEFT_CONTROL, deltaTime);

    //Render Scene
    render();
  }
  cleanUp();
} 


void SmallRenderer::render() {
  // Clear the screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(m_shaderProgram);
  for (const auto& obj : m_sceneObjects) {
    glBindVertexArray(obj.vao);

    // Get uniform locations
    GLuint MID = glGetUniformLocation(m_shaderProgram, "M");
    GLuint VID = glGetUniformLocation(m_shaderProgram, "V");
    GLuint PID = glGetUniformLocation(m_shaderProgram, "P");
      
    // Get uniform locations
    GLuint lightPosID = glGetUniformLocation(m_shaderProgram, "lightPos");
    GLuint timeID = glGetUniformLocation(m_shaderProgram, "fTime");
    GLuint viewPosID = glGetUniformLocation(m_shaderProgram, "viewPos");
    GLuint shininessID = glGetUniformLocation(m_shaderProgram, "shininess");
      
    // Set uniform values
    glUniform3f(lightPosID, 0.0f, 1.0f, 0.0f); // Light position
    glUniform3f(viewPosID, 0.0f, 0.0f, 10.0f); // Camera position
    glUniform1f(shininessID, 32.0f); // Shininess factor

    // Create model, view, projection matrices
    glm::mat4 M = glm::mat4(1.0f); // Identity matrix for model
    glm::mat4 V = m_camera.getViewMatrix(); // Use camera's view matrix
    glm::mat4 P = glm::perspective(
				   glm::radians(45.0f), // FOV
				   (float)m_width / (float)m_height, // Aspect ratio
				   0.1f, 100.0f // Near and far planes
				   );

    // Set uniforms
    glUniformMatrix4fv(MID, 1, GL_FALSE, &M[0][0]);
    glUniformMatrix4fv(VID, 1, GL_FALSE, &V[0][0]);
    glUniformMatrix4fv(PID, 1, GL_FALSE, &P[0][0]);
      
    // glUniform4f(lightPosID, 0.0f, 1.0f, 0.0f, 1.0f);
    // glUniform1f(timeID, (float)glfwGetTime()); // Current time

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, obj.vertexBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, obj.normalBuffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Bind the element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.elementBuffer);
  
    glDrawElements(GL_TRIANGLES, obj.numIndices, GL_UNSIGNED_INT, 0);
    checkGLError("glDrawElements");
  }
  // Swap buffers
  glfwSwapBuffers(m_window);
  glfwPollEvents();
}

void SmallRenderer::initShader(){
  m_shaderProgram = LoadShaders("shader/vertex.glsl", "shader/fragment.glsl");
  if (!m_shaderProgram) {
    throw std::runtime_error("Failed to load shaders");
  }
}
/**
   v  * Function to:
   * - free memory
   * - terminate glfw
   */
void SmallRenderer::cleanUp() {
  for (auto object : m_sceneObjects)
    glDeleteVertexArrays(1, &object.vao);
    
  glDeleteProgram(m_shaderProgram);
  glfwTerminate();
}

void SmallRenderer::checkGLError(const char* operation){
  GLenum error;
  while ((error = glGetError()) != GL_NO_ERROR) {
    std::cerr << "OpenGL error after " << operation << ": " << error << std::endl;
  }
}

// Mouse button callback
void SmallRenderer::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
  SmallRenderer* renderer = static_cast<SmallRenderer*>(glfwGetWindowUserPointer(window));
  if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
    if (action == GLFW_PRESS) {
      renderer->m_mouseMiddlePressed = true;
      double xpos, ypos;
      glfwGetCursorPos(window, &xpos, &ypos);
      renderer->m_lastMousePos = glm::vec2(xpos, ypos);
    } else if (action == GLFW_RELEASE) {
      renderer->m_mouseMiddlePressed = false;
    }
  }
}

// Cursor position callback
void SmallRenderer::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
  SmallRenderer* renderer = static_cast<SmallRenderer*>(glfwGetWindowUserPointer(window));
  if (renderer->m_mouseMiddlePressed) {
    glm::vec2 currentPos(xpos, ypos);
    glm::vec2 delta = currentPos - renderer->m_lastMousePos;
    renderer->m_lastMousePos = currentPos;

    // Rotate the camera based on mouse movement
    renderer->m_camera.processMouseMovement(delta.x, -delta.y); // Invert y to match screen coordinates
  }
}

void SmallRenderer::framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  SmallRenderer* renderer = static_cast<SmallRenderer*>(glfwGetWindowUserPointer(window));
  // make sure the viewport matches the new window dimensions; note that width and 
  // height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
  renderer->m_height = height;
  renderer->m_width = width;
  // Re-render the scene because the current frame was drawn for the old resolution
  renderer->render();
}
