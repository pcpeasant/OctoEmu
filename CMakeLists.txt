cmake_minimum_required(VERSION 3.8)

project(OctoSim LANGUAGES C)

add_executable(OctoSim ./src/main.c ./src/core.c)
target_include_directories(OctoSim PUBLIC ./include
                            ./external/raylib-5.0_linux_amd64/include)
target_link_libraries(OctoSim m ${PROJECT_SOURCE_DIR}/external/raylib-5.0_linux_amd64/lib/libraylib.a)
target_compile_options(OctoSim PUBLIC -Wall -Wextra)
