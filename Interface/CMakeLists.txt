cmake_minimum_required(VERSION 3.20)
project(Interface)

file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_library(${PROJECT_NAME} STATIC ${SOURCES})

add_library(Interface::library ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} PUBLIC
    AudioLib::library
    glfw
    glad
    imgui::imgui
    ImGuiFileDialog
)
