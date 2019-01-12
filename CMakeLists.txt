cmake_minimum_required(VERSION 3.2)
project(GBC)

find_package(SDL2 REQUIRED SDL2)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -ggdb")
add_executable(GBC main.cpp mem.cpp mem.h platform.cpp platform.h cpu.cpp cpu.h graphics_display.cpp graphics_display.h video.cpp video.h)
target_link_libraries(GBC PRIVATE SDL2)