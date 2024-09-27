cmake_minimum_required(VERSION 3.0.0)
project(voxelengine VERSION 0.1.0 LANGUAGES C)

include(CTest)
enable_testing()

add_executable(voxelengine src/main.c src/glad/glad.h src/glad/glad.c src/octree/octree.h src/octree/octree.c)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

target_link_libraries(voxelengine glfw m)
