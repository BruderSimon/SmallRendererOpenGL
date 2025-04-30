include(FetchContent)

# FetchContent
FetchContent_Declare(
  glew
  URL                         https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.zip
  SOURCE_DIR                  ${CMAKE_SOURCE_DIR}/dependencies/glew
  DOWNLOAD_EXTRACT_TIMESTAMP  TRUE
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

FetchContent_Declare(
  imgui
  GIT_REPOSITORY        https://github.com/ocornut/imgui.git
  GIT_TAG               f5befd2d29e66809cd1110a152e375a7f1981f06
  SOURCE_DIR            ${CMAKE_SOURCE_DIR}/dependencies/imgui
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

FetchContent_MakeAvailable(glew)
FetchContent_MakeAvailable(glm)
FetchContent_MakeAvailable(glfw)
FetchContent_MakeAvailable(imgui)

include_directories(
  glm/
  glfw/include/GLFW
  glew/include/
)

# Dear ImGui
add_library(imgui)
set(IMGUI_DIR imgui/)
set(IMGUI_SRC
  ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
  ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_demo.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp
)
target_include_directories(imgui
  PUBLIC
  ${IMGUI_DIR}
  ${IMGUI_DIR}/backends
)
target_sources(imgui PUBLIC ${IMGUI_SRC})
target_link_libraries(imgui glfw)

#GLEW
set(GLEW_SOURCE
  glew/src/glew.c
)

add_library(GLEW_220 STATIC
  ${GLEW_SOURCE}
  ${GLEW_INCLUDE}
)

target_link_libraries(GLEW_220
  ${OPENGL_LIBRARY}
  ${EXTRA_LIBS}
)
