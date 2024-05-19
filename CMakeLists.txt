cmake_minimum_required(VERSION 3.8)

project(Chippy VERSION 1.1 LANGUAGES C)

add_executable(Chippy ${PROJECT_SOURCE_DIR}/src/main.c)
target_include_directories(Chippy PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/external/raylib-5.0_linux_amd64/include)
target_link_libraries(Chippy m ${PROJECT_SOURCE_DIR}/external/raylib-5.0_linux_amd64/lib/libraylib.a)