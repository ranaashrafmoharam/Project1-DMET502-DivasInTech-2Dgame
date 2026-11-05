cmake_minimum_required(VERSION 3.16)
project(NinjagoGame CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

include_directories(
    src
    External/include
)

file(GLOB_RECURSE GAME_SOURCES
    src/*.cpp
)

set(ALL_SOURCES
    ${GAME_SOURCES}
    External/src/glad.c
)

add_executable(NinjagoGame ${ALL_SOURCES})

target_link_libraries(NinjagoGame PRIVATE
    glfw
    opengl32
    winmm
)

# Copy assets to build directory so shaders/sprites are directly found
add_custom_command(TARGET NinjagoGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:NinjagoGame>/assets
)
