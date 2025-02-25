#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "common/tiny_obj_loader.h"
#include <glm/gtc/matrix_transform.hpp>

#include <string>

struct SceneObject {
  GLuint vao;
  GLuint vertexBuffer;
  GLuint normalBuffer;
  GLuint elementBuffer;
  GLuint textureID;
  size_t numIndices;
  
  glm::mat4 modelMatrix;
  tinyobj::material_t material;
  
  std::vector<tinyobj::material_t> m_materials;
  std::map<std::string, GLuint> m_textures;

  void loadModel(std::string &path);
  void loadTexture(std::string &path);
  void loadObject(std::string& path, std::string& mtlPath);
};


#endif
