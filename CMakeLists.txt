cmake_minimum_required(VERSION 3.8)

project(OctoEmu LANGUAGES C)

add_executable(OctoEmu ./src/main.c ./src/core.c)
target_include_directories(OctoEmu PUBLIC ./include
                            ./external/raylib-5.0_linux_amd64/include)
target_link_libraries(OctoEmu m ${PROJECT_SOURCE_DIR}/external/raylib-5.0_linux_amd64/lib/libraylib.a)
target_compile_options(OctoEmu PUBLIC -Wall -Wextra)
