cmake_minimum_required(VERSION 3.16)
project(Member3_NinjagoPreview LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

add_executable(Member3Preview
    src/main.cpp
    src/core/Texture.cpp
    src/scene/Scene.cpp
    src/scene/EnvironmentElements.cpp
    src/scene/HomeBase.cpp
    src/scene/PropModels.cpp
    src/scene/LaneHighlight.cpp
    src/cutscene/Cutscene.cpp
    src/ui/PlayerCards.cpp
    src/ui/HUD.cpp
    src/entities/Collectible.cpp
    src/ui/HealthBar.cpp
    External/glad/src/glad.c
)

 target_include_directories(Member3Preview PRIVATE
    src
    External/glad/include
)
target_link_libraries(Member3Preview PRIVATE glfw)

if(WIN32)
    target_link_libraries(Member3Preview PRIVATE opengl32)
endif()
