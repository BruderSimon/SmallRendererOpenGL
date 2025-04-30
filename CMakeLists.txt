cmake_minimum_required(VERSION 3.30)

project(SmallRenderOpenGL VERSION 0.1.0)

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

# Compiler-specific optimization flags
if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
  if(MSVC)
    # MSVC-specific optimization flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /Ob2 /Oi /Ot /GF")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:SSE2")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LTCG") # Link-time code generation
  else()
    # GCC/Clang optimization flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3") # Highest optimization level
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native") # Optimize for current CPU
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -ffast-math") # Aggressive floating-point optimizations
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -funroll-loops") # Unroll loops
  endif()
else()
  # Set Debug Flags
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb") # Debug symbols
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0") # No optimization for better debugging
  # set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CSS_FLAGS_DEBUG} -Werror") # Make all warnings into errors
  # set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CSS_FLAGS_DEBUG} -pedantic")# all warnings demanded by strict ISO C/C++;
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# For Clangd langage server
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Packages 
find_package(OpenGL REQUIRED)

add_subdirectory(dependencies)

include_directories(
  dependencies/glm/
  dependencies/glfw/include/GLFW/
  dependencies/glew/include/
  dependencies/imgui/
  dependencies/imgui/backends/
)

set(ALL_LIBS
  ${OPENGL_LIBRARY}
  GLEW_220
  glfw
  glm::glm
  imgui
)

add_executable(SmallRendererOpenGL
  src/smallrender.cpp
  src/sceneobject.cpp
  src/main.cpp
  
  src/common/shader.cpp
  src/common/stb_image.cpp

  shader/vertex.glsl
  shader/fragment.glsl
)

target_link_libraries(SmallRendererOpenGL ${ALL_LIBS})

# To Copy the Compile Commands in the source directory
add_custom_command(
  TARGET SmallRendererOpenGL
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/compile_commands.json"
)

#Copy the Executabel
add_custom_command(
  TARGET SmallRendererOpenGL
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy "SmallRendererOpenGL${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_SOURCE_DIR}/"
)
