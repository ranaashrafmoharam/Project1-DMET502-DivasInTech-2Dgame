cmake_minimum_required(VERSION 3.20)
project(Project1Dmet502 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OPENGL_TEMPLATE_ROOT "" CACHE PATH "Path to OpenGL Template for VS Code (Other)")
if (NOT OPENGL_TEMPLATE_ROOT)
    message(FATAL_ERROR "Set OPENGL_TEMPLATE_ROOT to the provided OpenGL template folder.")
endif()

set(TEMPLATE_EXTERNAL "${OPENGL_TEMPLATE_ROOT}/external")
foreach(required_path
        "${TEMPLATE_EXTERNAL}/glfw-3.5.1/CMakeLists.txt"
        "${TEMPLATE_EXTERNAL}/glm/CMakeLists.txt"
        "${CMAKE_CURRENT_SOURCE_DIR}/External/glad/include/glad/glad.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/External/glad/src/glad.c")
    if (NOT EXISTS "${required_path}")
        message(FATAL_ERROR "Required template dependency is missing: ${required_path}")
    endif()
endforeach()

add_subdirectory("${TEMPLATE_EXTERNAL}/glfw-3.5.1" "${CMAKE_BINARY_DIR}/glfw")
add_subdirectory("${TEMPLATE_EXTERNAL}/glm" "${CMAKE_BINARY_DIR}/glm")
find_package(OpenGL REQUIRED)

file(GLOB_RECURSE SHARED_GAME_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
list(FILTER SHARED_GAME_SOURCES EXCLUDE REGEX "[/\\\\](main|game_main|glad)\\.cpp$")

set(GAME_INCLUDE_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/glad/include")

add_executable(Project1Dmet502
    src/game_main.cpp
    ${SHARED_GAME_SOURCES}
    External/glad/src/glad.c)
add_executable(Member3Preview
    src/main.cpp
    ${SHARED_GAME_SOURCES}
    External/glad/src/glad.c)

foreach(target Project1Dmet502 Member3Preview)
    target_include_directories(${target} PRIVATE ${GAME_INCLUDE_DIRS})
    target_link_libraries(${target} PRIVATE glfw glm::glm OpenGL::GL)
endforeach()
