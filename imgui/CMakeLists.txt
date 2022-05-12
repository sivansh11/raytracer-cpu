cmake_minimum_required(VERSION 3.0)
project(Imgui)

set(IMGUI_DIR imgui)

file(GLOB SRC_FILES ${IMGUI_DIR}/*cpp)

set(BACKENDS ${IMGUI_DIR}/backends)

include_directories(${IMGUI_DIR})
include_directories(${BACKENDS})
# include_directories(backends)

set(BACKEND_SRC_FILES
    ${BACKENDS}/imgui_impl_opengl3.cpp
    ${BACKENDS}/imgui_impl_glfw.cpp
)

add_library(imgui ${SRC_FILES} ${BACKEND_SRC_FILES})

target_include_directories(imgui
    PUBLIC imgui/
    PUBLIC imgui/backends

    # Fix for not finding GLFW/glfw3.h. There's probably a better way to solve this.
    "../glfw/include"
)


# message(STATUS "foo include dir: ${CMAKE_CURRENT_SOURCE_DIR}")
# message(STATUS "foo include dir: ${THIRDPARTY_PATH}")




