project(chitralok)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_BUILD_TYPE Debug)

#find_package(GLEW REQUIRED)
#find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

#include_directories(${GLEW_INCLUDE_DIRS})
#include_directories(${OPENGL_INCLUDE_DIR})  # special case
include_directories(${SDL2_INCLUDE_DIRS})

# src
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB_RECURSE SOURCES "src/*.c")
file(GLOB_RECURSE HEADERS "src/*.h")



#message(STATUS "sources are ${SOURCES}")

add_executable(chitralok ${SOURCES} ${HEADERS})
#target_link_libraries(vilasita SDL2::SDL2 ${GLEW_LIBRARIES} ${OPENGL_LIBRARY}  ${ASSIMP_LIBRARIES})
target_link_libraries(chitralok SDL2::SDL2)

