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
        "${TEMPLATE_EXTERNAL}/glad/include/glad/gl.h")
    if (NOT EXISTS "${required_path}")
        message(FATAL_ERROR "Required template dependency is missing: ${required_path}")
    endif()
endforeach()

add_subdirectory("${TEMPLATE_EXTERNAL}/glfw-3.5.1" "${CMAKE_BINARY_DIR}/glfw")
add_subdirectory("${TEMPLATE_EXTERNAL}/glm" "${CMAKE_BINARY_DIR}/glm")
find_package(OpenGL REQUIRED)

file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_executable(Project1Dmet502 ${GAME_SOURCES})
target_include_directories(Project1Dmet502 PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${TEMPLATE_EXTERNAL}/glad/include")
target_link_libraries(Project1Dmet502 PRIVATE glfw glm::glm OpenGL::GL)
