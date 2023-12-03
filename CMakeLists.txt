cmake_minimum_required(VERSION 3.2)
project(AoC VERSION 2022)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_C_STANDARD 11)
set(
    CMAKE_C_FLAGS 
    "-ggdb -Wall -rdynamic -fstack-clash-protection -fasynchronous-unwind-tables\
    -fstack-protector-all -fcf-protection -pipe"
)

include_directories(include/)

file(GLOB sources src/*.c src/days/*.c src/extra/*.c)
add_executable(${CMAKE_PROJECT_NAME} ${sources})
target_link_libraries(${CMAKE_PROJECT_NAME} ncurses)
target_link_libraries(${CMAKE_PROJECT_NAME} dl)

add_custom_command(
    TARGET ${CMAKE_PROJECT_NAME} PRE_BUILD
    # COMMAND ${CMAKE_COMMAND} -E create_symlink
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/inputs $<TARGET_FILE_DIR:${PROJECT_NAME}>/inputs
)
