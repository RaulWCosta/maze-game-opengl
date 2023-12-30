cmake_minimum_required(VERSION 3.5)

project(maze_game VERSION 0.1.0)

include_directories(${CMAKE_SOURCE_DIR}/dependencies/include)
# include_directories(${CMAKE_SOURCE_DIR}/src)

add_library(glad STATIC ${CMAKE_SOURCE_DIR}/dependencies/include/glad/glad.c)
add_library(stb_image STATIC ${CMAKE_SOURCE_DIR}/dependencies/include/stb_image/stb_image.cpp)

find_package(OpenGL REQUIRED)

add_executable(maze_game src/main.cpp)

target_link_libraries(maze_game glfw glad stb_image)
