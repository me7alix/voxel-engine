cmake_minimum_required(VERSION 3.0.0)
project(voxelengine VERSION 0.1.0 LANGUAGES C)

include(CTest)
enable_testing()

add_executable(voxelengine src/main.c src/Glad/glad.h src/Glad/glad.c src/Octree/octree.h src/Octree/octree.c)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

target_link_libraries(voxelengine glfw m)
