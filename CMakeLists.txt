cmake_minimum_required(VERSION 3.8)

project(Chippy LANGUAGES C)

add_executable(Chippy ./src/main.c ./src/core.c ./src/embed.c)
target_include_directories(Chippy PUBLIC ./include
                            ./external/raylib-5.0_linux_amd64/include)
target_link_libraries(Chippy m ./external/raylib-5.0_linux_amd64/lib/libraylib.a)
target_compile_options(Chippy PUBLIC -Wall -Wextra)