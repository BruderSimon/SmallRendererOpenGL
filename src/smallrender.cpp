#include "smallrender.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "common/shader.hpp"
#include "common/stb_image.h"
#include <glm/gtc/matrix_transform.hpp>
#include "common/tiny_obj_loader.h"

#include <cstddef>
#include <cstring>
#include <iostream>
#include <stdexcept>

void SmallRenderer::init(std::string& model, std::string mtl){
  initWindow();
  loadModel(model, mtl);
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
  glfwMakeContextCurrent(m_window); // Initialize GLEW
  if(glewInit() != GLEW_OK) {
    glfwTerminate();
    throw std::runtime_error("Failed to initialize GLEW");
  }
  glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
  glfwSetCursorPosCallback(m_window, cursorPosCallback);
  glfwSetWindowUserPointer(m_window, this);
  // Ensure we can capture keys
  glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_TRUE);
  glClearColor(0.0f, 0.1f, 0.3f, 0.0f);

  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);
}

void SmallRenderer::run(){
  
  while(glfwGetKey(m_window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
	glfwWindowShouldClose(m_window) == 0 ){
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
    render();
    // Swap buffers
    glfwSwapBuffers(m_window);
    glfwPollEvents();
  }
  cleanUp();
} 

void SmallRenderer::loadModel(std::string& path, std::string mtlPath) {
  tinyobj::ObjReaderConfig reader_config;
  reader_config.mtl_search_path = mtlPath;
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
  m_materials = reader.GetMaterials();
  
  // Vectors to store the vertex data
  std::vector<float> vertices;
  std::vector<float> normals;
  std::vector<unsigned int> indices;
  std::vector<int> material_ids;

  // Loop over shapes
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
  
  m_numIndices = indices.size();
  std::cout << "Loaded: " << vertices.size()/3 << " vertices, " << normals.size()/3 << " normals\n";
  
  // Generate and bind VAO
  glGenVertexArrays(1, &m_vao);
  glBindVertexArray(m_vao);

  // Generate and bind Vertex buffer
  glGenBuffers(1, &m_vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glEnableVertexAttribArray(0);
  
  // Generate and bind Normal buffer
  glGenBuffers(1, &m_normalBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer);
  glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glEnableVertexAttribArray(1);

  // Generate and bind Element buffer
  glGenBuffers(1, &m_elementBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

  std::cout << "Vertex buffer size: " << vertices.size() * sizeof(float) << " bytes\n";
  std::cout << "Normal buffer size: " << normals.size() * sizeof(float) << " bytes\n";
  std::cout << "Index count: " << m_numIndices << "\n";
}
void SmallRenderer::loadTextureFromFile(const std::string& filename, GLuint textureID) {
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

void SmallRenderer::render() {
  
  glUseProgram(m_shaderProgram);
  checkGLError("glUseProgram");
  glBindVertexArray(m_vao);
  checkGLError("glBindVertexArray");
  //glDispatchCompute(m_width, m_height, 1);
  //glMemoryBarrier(GL_ALL_BARRIER_BITS);


  if (!m_materials.empty()) {
        // Set material properties
        GLuint materialAmbientID = glGetUniformLocation(m_shaderProgram, "materialAmbient");
        GLuint materialDiffuseID = glGetUniformLocation(m_shaderProgram, "materialDiffuse");
        GLuint materialSpecularID = glGetUniformLocation(m_shaderProgram, "materialSpecular");
        GLuint materialShininessID = glGetUniformLocation(m_shaderProgram, "materialShininess");
        
        glUniform3fv(materialAmbientID, 1, m_materials[0].ambient);
        glUniform3fv(materialDiffuseID, 1, m_materials[0].diffuse);
        glUniform3fv(materialSpecularID, 1, m_materials[0].specular);
        glUniform1f(materialShininessID, m_materials[0].shininess);
        
        // Bind texture if available
        if (!m_materials[0].diffuse_texname.empty()) {
            GLuint textureID = glGetUniformLocation(m_shaderProgram, "textureSampler");
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_textures[m_materials[0].diffuse_texname]);
            glUniform1i(textureID, 0);
        }
    }
  
  // Get uniform locations
  GLuint MID = glGetUniformLocation(m_shaderProgram, "M");
  GLuint VID = glGetUniformLocation(m_shaderProgram, "V");
  GLuint PID = glGetUniformLocation(m_shaderProgram, "P");
  GLuint lightPosID = glGetUniformLocation(m_shaderProgram, "lightPos");
  GLuint timeID = glGetUniformLocation(m_shaderProgram, "fTime");

  // Create model, view, projection matrices
  glm::mat4 M = m_modelRotation;
  glm::mat4 V = glm::lookAt(
			    glm::vec3(0,0,10), // Camera position
			    glm::vec3(0,0,0), // Look at origin
			    glm::vec3(0,1,0)  // Up vector
			    );
  glm::mat4 P = glm::perspective(
				 glm::radians(45.0f), // FOV
				 (float)m_width / (float)m_height, // Aspect ratio
				 0.1f, 100.0f // Near and far planes
				 );

  // Set uniforms
  glUniformMatrix4fv(MID, 1, GL_FALSE, &M[0][0]);
  glUniformMatrix4fv(VID, 1, GL_FALSE, &V[0][0]);
  glUniformMatrix4fv(PID, 1, GL_FALSE, &P[0][0]);
  glUniform4f(lightPosID, 0.0f, 1.0f, 0.0f, 1.0f);
  glUniform1f(timeID, (float)glfwGetTime()); // Current time

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  // Bind the element buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBuffer);
  
  glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, 0);
  checkGLError("glDrawElements");
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
void SmallRenderer::cleanUp(){
  glDeleteVertexArrays(1, &m_vao);
  glDeleteProgram(m_shaderProgram);
  glfwTerminate();
}
void SmallRenderer::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    SmallRenderer* renderer = static_cast<SmallRenderer*>(glfwGetWindowUserPointer(window));
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            renderer->m_mousePressed = true;
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            renderer->m_lastMousePos = glm::vec2(xpos, ypos);
        } else if (action == GLFW_RELEASE) {
            renderer->m_mousePressed = false;
        }
    }
}

void SmallRenderer::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    SmallRenderer* renderer = static_cast<SmallRenderer*>(glfwGetWindowUserPointer(window));
    if (renderer->m_mousePressed) {
        glm::vec2 currentPos(xpos, ypos);
        glm::vec2 delta = currentPos - renderer->m_lastMousePos;
        
        // Update rotation matrix
        float rotationSpeed = 0.01f;
        glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), rotationSpeed * delta.y, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), rotationSpeed * delta.x, glm::vec3(0.0f, 1.0f, 0.0f));
        
        renderer->m_modelRotation = rotationY * rotationX * renderer->m_modelRotation;
        renderer->m_lastMousePos = currentPos;
    }
}

void SmallRenderer::checkGLError(const char* operation){
  GLenum error;
  while ((error = glGetError()) != GL_NO_ERROR) {
    std::cerr << "OpenGL error after " << operation << ": " << error << std::endl;
  }
}
