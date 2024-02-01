#include "smallrender.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "common/tiny_obj_loader.h"
#include "common/shader.hpp"

#include <cstddef>
#include <cstring>
#include <iostream>
#include <stdexcept>

void SmallRenderer::init(std::string& model){
  initWindow();
  loadModel(model);
  initShader();
  
}

void SmallRenderer::initWindow(){
  glewExperimental = true;
  if(!glfwInit())
    throw std::runtime_error("Failed to initialize GLFW");
 
  glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

  m_window = glfwCreateWindow( m_width, m_height, "Small Renderer", NULL, NULL);
  if(m_window == NULL){
    glfwTerminate();
    throw std::runtime_error("Failed to open Window");
  }
  glfwMakeContextCurrent(m_window); // Initialize GLEW
  if(glewInit() != GLEW_OK) {
    glfwTerminate();
    throw std::runtime_error("Failed to initialize GLEW");
}
  // Ensure we can capture keys
  glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_TRUE);

  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);
}

void SmallRenderer::run(){

  glUseProgram(m_shaderProgram);
  
  while(glfwGetKey(m_window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
	glfwWindowShouldClose(m_window) == 0 ){
    glClear( GL_COLOR_BUFFER_BIT );

    render();
    // Swap buffers
    glfwSwapBuffers(m_window);
    glfwPollEvents();
  }
  cleanUp();
} 

void SmallRenderer::loadModel(std::string& path) {
    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = "./"; // Path to material files
    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(path, reader_config)) {
        if (!reader.Error().empty()) 
            std::cerr << "TinyObjReader: " << reader.Error();
        throw std::runtime_error("Failed to Load Object");
    }
    if (!reader.Warning().empty())
        std::cout << "TinyObjReader: " << reader.Warning();

    auto& m_attributes = reader.GetAttrib();
    auto& m_shapes = reader.GetShapes();
    auto& m_materials = reader.GetMaterials();

    // Vectors to store the vertex data
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texcoords;
    std::vector<unsigned int> indices;

    // Loop over shapes
    for (const auto& shape : m_shapes) {
        size_t index_offset = 0;
        
        // Loop over faces(polygon)
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            size_t fv = size_t(shape.mesh.num_face_vertices[f]);

            // Loop over vertices in the face
            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
                
                // Vertices
                vertices.push_back(m_attributes.vertices[3 * idx.vertex_index + 0]);
                vertices.push_back(m_attributes.vertices[3 * idx.vertex_index + 1]);
                vertices.push_back(m_attributes.vertices[3 * idx.vertex_index + 2]);

                // Normals
                if (idx.normal_index >= 0) {
                    normals.push_back(m_attributes.normals[3 * idx.normal_index + 0]);
                    normals.push_back(m_attributes.normals[3 * idx.normal_index + 1]);
                    normals.push_back(m_attributes.normals[3 * idx.normal_index + 2]);
                } else {
                    // Add default normal if none exists
                    normals.push_back(0.0f);
                    normals.push_back(0.0f);
                    normals.push_back(1.0f);
                }

                // Texture coordinates
                if (idx.texcoord_index >= 0) {
                    texcoords.push_back(m_attributes.texcoords[2 * idx.texcoord_index + 0]);
                    texcoords.push_back(m_attributes.texcoords[2 * idx.texcoord_index + 1]);
                } else {
                    // Add default texture coordinates if none exists
                    texcoords.push_back(0.0f);
                    texcoords.push_back(0.0f);
                }

                // Add indices
                indices.push_back(index_offset + v);
            }
            index_offset += fv;
        }
    }

    // Generate and bind VAO
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // Generate and bind buffers
    GLuint vbo[3], ebo;
    glGenBuffers(3, vbo);
    glGenBuffers(1, &ebo);

    // Vertices
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // Normals
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // Texture coordinates
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof(float), texcoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    // Indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Store the number of indices for drawing
    m_numIndices = indices.size();

    // Unbind VAO
    glBindVertexArray(0);
}

void SmallRenderer::render() {
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void SmallRenderer::initShader(){
  m_shaderProgram = LoadShaders("vertex.glsl", "fragment.glsl");
  if (!m_shaderProgram) {
    throw std::runtime_error("Failed to load shaders");
  }
}
  /**
   * Function to:
   * - free memory
   * - terminate glfw
   */
  void SmallRenderer::cleanUp(){
    glfwTerminate();
  }
