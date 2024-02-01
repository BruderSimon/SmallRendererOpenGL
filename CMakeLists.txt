cmake_minimum_required(VERSION 3.5)
project(SmallRenderOpenGL)

# Checks
if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
    message( FATAL_ERROR "Please select another Build Directory ! (Seperate Build and Source direcotries)" )
endif()
if( CMAKE_SOURCE_DIR MATCHES " " )
	message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()
if( CMAKE_BINARY_DIR MATCHES " " )
  message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()

# For Clangd langage server
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# Packages 
find_package(OpenGL REQUIRED)
include(FetchContent)

# FetchContent
FetchContent_Declare(
  glew
  URL                         https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.zip
  SOURCE_DIR                  ${CMAKE_SOURCE_DIR}/dependencies/glew
  DOWNLOAD_EXTRACT_TIMESTAMP  FALSE
)
FetchContent_Declare(
  glm
  GIT_REPOSITORY	https://github.com/g-truc/glm.git
  GIT_TAG 	        0af55ccecd98d4e5a8d1fad7de25ba429d60e863 # 1.0.1
  SOURCE_DIR            ${CMAKE_SOURCE_DIR}/dependencies/glm
)
FetchContent_Declare(
  glfw
  GIT_REPOSITORY        https://github.com/glfw/glfw.git
  GIT_TAG               7b6aead9fb88b3623e3b3725ebb42670cbe4c579 # 3.4
  SOURCE_DIR            ${CMAKE_SOURCE_DIR}/dependencies/glfw
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
	-DGLEW_NO_GLU	
)

#FetchContent_MakeAvailable(glew)
FetchContent_MakeAvailable(glm)
FetchContent_MakeAvailable(glfw)

add_subdirectory(dependencies)
include_directories(
  dependencies/glm/
  dependencies/glfw/include/GLFW/
  dependencies/glew/include/
)

set(ALL_LIBS
  ${OPENGL_LIBRARY}
  GLEW_220
  glfw
  glm::glm
)

add_executable(SmallRendererOpenGL
  src/smallrender.cpp
  src/main.cpp
  
  src/common/shader.cpp

  shader/vertex.glsl
  shader/fragment.glsl
)

target_link_libraries(SmallRendererOpenGL ${ALL_LIBS})

#Copy the Executabel
add_custom_command(
  TARGET SmallRendererOpenGL
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy "SmallRendererOpenGL${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_SOURCE_DIR}/"
)
