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

if (WIN32)
	cmake_policy(SET CMP0079 NEW)
	target_link_libraries(imgui PRIVATE glfw)
endif()

target_link_libraries(${PROJECT_NAME}
    glm
    glad
    glfw
    imgui
)

if (NOT WIN32)
	# note: might not actually be necessary!
	target_link_libraries(${PROJECT_NAME} pthreads)
endif()