cmake_minimum_required(VERSION 3.10)

project(app)

file(GLOB SRC_FILES src/*.cpp)

add_subdirectory(glm)
add_subdirectory(glad)
add_subdirectory(glfw)
add_subdirectory(imgui)

add_executable(${PROJECT_NAME} ${SRC_FILES})

include_directories(${PROJECT_NAME}
    include
)

target_link_libraries(${PROJECT_NAME}
    glm
    glad
    glfw
    imgui
    pthread
)
