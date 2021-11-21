cmake_minimum_required(VERSION 3.15)

project(Pacman)

find_package(OpenGL REQUIRED)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(glm)
add_subdirectory(glfw)
add_subdirectory(glad)
add_subdirectory(tinyobjloader)

add_executable(Pacman
	main.cpp
	shaders/playerShad.h
	shaders/mapShad.h
    shaders/ghostShad.h
	shaders/pelletsShad.h 
	shaders/modelShader.h
	
	include/glutil.h
	"character.h"
	"character.cpp"
	"pellet.h"
	"pellet.cpp"
	"map.h" 
	"map.cpp"
	"include.h"
	"globFunc.h"
	"globFunc.cpp"
	"initialize.h"
	"initialize.cpp"
	"ghost.h"
	"ghost.cpp"
	"pacman.h"
	"pacman.cpp"
	"camera.h"
	"camera.cpp" )

target_link_libraries(Pacman
	PRIVATE
	glad
	glfw
	glm
	tinyobjloader
	OpenGL::GL)

  target_include_directories(${PROJECT_NAME}
  PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/stb/include)
  
target_compile_definitions(${PROJECT_NAME}
  PRIVATE
  STB_IMAGE_IMPLEMENTATION)

  add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  ${CMAKE_SOURCE_DIR}/assets/pacman.png
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/pacman.png)

    add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  ${CMAKE_SOURCE_DIR}/assets/wallTexture.png
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/wallTexture.png)

      add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  ${CMAKE_SOURCE_DIR}/assets/ghostModelShader.png
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/ghostModelShader.png)