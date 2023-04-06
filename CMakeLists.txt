cmake_minimum_required(VERSION 3.0.0)

project(FINALPROJECT178)
find_package(raylib REQUIRED)
# add_executable(Mazegen Mazegen.c)
# target_include_directories(Mazegen PRIVATE ${raylib_INCLUDE_DIRS})
# target_link_libraries(Mazegen PRIVATE ${raylib_LIBRARIES})
# add_executable(Mazesolve Mazesolve.c)
# target_include_directories(Mazesolve PRIVATE ${raylib_INCLUDE_DIRS})
# target_link_libraries(Mazesolve PRIVATE ${raylib_LIBRARIES})
add_executable(Maze Maze.cpp)
target_include_directories(Maze PRIVATE ${raylib_INCLUDE_DIRS})
target_link_libraries(Maze PRIVATE ${raylib_LIBRARIES})

