cmake_minimum_required(VERSION 3.20)
project(CHIP-8 C)

set(CMAKE_C_STANDARD 23)

find_package(raylib REQUIRED)

add_executable(CHIP-8 main.c core.c gui.c)

target_link_libraries(CHIP-8 PRIVATE raylib)