cmake_minimum_required(VERSION 3.5.1)
project(maze)
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_LIBRARIES "-lSDL2 -lSDL2main -lGL -ldl -I/usr/include/SDL2/")

include_directories(glad)
add_subdirectory(glm)

file(GLOB SRC
        glad/*.c
        *.cpp
        *.h)


add_executable(maze ${SRC})
target_link_libraries(${PROJECT_NAME} SDL2main SDL2)
