#include "sceneobject.h"
#include "common/stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "common/tiny_obj_loader.h"

#include <stdexcept>
#include <iostream>

void SceneObject::loadObject(std::string& path, std::string& mtlPath) {
  loadModel(path);
  
}

void SceneObject::loadModel(std::string &path) {
  tinyobj::ObjReaderConfig reader_config;
  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(path, reader_config)) {
    if (!reader.Error().empty()) 
      std::cerr << "TinyObjReader: " << reader.Error();
    throw std::runtime_error("Failed to Load Object");
  }
  if (!reader.Warning().empty())
    std::cout << "TinyObjReader: " << reader.Warning();

  auto& attrib = reader.GetAttrib();
  auto& shapes = reader.GetShapes();

  // Vectors to store the vertex data
  std::vector<float> vertices;
  std::vector<float> normals;
  std::vector<unsigned int> indices;
  std::vector<int> material_ids;

  for (const auto& shape : shapes) {
    // Loop over faces (polygon)
    size_t index_offset = 0;
    for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
      size_t fv = shape.mesh.num_face_vertices[f];
      
      // Loop over vertices in the face
      for (size_t v = 0; v < fv; v++) {
        // access to vertex
        tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
        
        // vertex position
        vertices.push_back(attrib.vertices[3 * idx.vertex_index + 0]);
        vertices.push_back(attrib.vertices[3 * idx.vertex_index + 1]);
        vertices.push_back(attrib.vertices[3 * idx.vertex_index + 2]);
        
        // vertex normals
        if (idx.normal_index >= 0) {
          normals.push_back(attrib.normals[3 * idx.normal_index + 0]);
          normals.push_back(attrib.normals[3 * idx.normal_index + 1]);
          normals.push_back(attrib.normals[3 * idx.normal_index + 2]);
        } else {
          // Default normal if not provided
          normals.push_back(0.0f);
          normals.push_back(0.0f);
          normals.push_back(1.0f);
        }
        // Add index (we're using a flat array, so indices are sequential)
        indices.push_back(index_offset + v);
	int material_id = shape.mesh.material_ids[f];
	material_ids.push_back(material_id);
      }
      index_offset += fv;
    }
  }
  numIndices = indices.size();
  std::cout << "Loaded: " << vertices.size()/3 << " vertices, " << normals.size()/3 << " normals\n";
  
  // Generate and bind VAO
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Generate and bind Vertex buffer
  glGenBuffers(1, &vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glEnableVertexAttribArray(0);
  
  // Generate and bind Normal buffer
  glGenBuffers(1, &normalBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
  glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glEnableVertexAttribArray(1);

  // Generate and bind Element buffer
  glGenBuffers(1, &elementBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

  std::cout << "Vertex buffer size: " << vertices.size() * sizeof(float) << " bytes\n";
  std::cout << "Normal buffer size: " << normals.size() * sizeof(float) << " bytes\n";
  std::cout << "Index count: " << numIndices << "\n";
}

void SceneObject::loadTexture(std::string &filename){
  int width, height, channels;
    unsigned char* image = stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb_alpha);
    
    if (!image) {
        std::cerr << "Failed to load texture: " << filename << std::endl;
        return;
    }
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    stbi_image_free(image);  
}
