#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

struct Camera{
  glm::vec3 position;
  glm::vec3 front;
  glm::vec3 up;
  float yaw;
  float pitch;
  
Camera() : position(glm::vec3(0.0f, 0.0f, 10.0f)),
    front(glm::vec3(0.0f, 0.0f, -1.0f)),
    up(glm::vec3(0.0f, 1.0f, 0.0f)),
    yaw(-90.0f), pitch(0.0f) {}
  
  glm::mat4 getViewMatrix() {return glm::lookAt(position, position + front, up);}

  void processKeyboard(int key, float deltaTime) {
    float velocity = 5.0f * deltaTime;
    if (key == GLFW_KEY_W)
      position += front * velocity;
    if (key == GLFW_KEY_S)
      position -= front * velocity;
    if (key == GLFW_KEY_A)
      position -= glm::normalize(glm::cross(front, up)) * velocity;
    if (key == GLFW_KEY_D)
      position += glm::normalize(glm::cross(front, up)) * velocity;
    if (key == GLFW_KEY_SPACE)
      position += up * velocity;
    if (key == GLFW_KEY_LEFT_CONTROL)
      position -= up * velocity;
  }

  void processMouseMovement(float xoffset, float yoffset) {
    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
      pitch = 89.0f;
    if (pitch < -89.0f)
      pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(direction);
  }
};

#endif
