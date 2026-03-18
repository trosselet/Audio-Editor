cmake_minimum_required(VERSION 3.20)
project(AudioProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(COPY ${PROJECT_SOURCE_DIR}/res/ DESTINATION ${PROJECT_BINARY_DIR}/res)

include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8
)
FetchContent_MakeAvailable(glfw)
add_subdirectory(3rdparty/glad)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG docking
)
FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_compile_definitions(imgui PUBLIC
    IMGUI_IMPL_OPENGL_LOADER_GLAD
    IMGUI_HAS_DOCKING=1
    IMGUI_HAS_VIEWPORT=1
)
target_link_libraries(imgui PUBLIC glfw)
add_library(imgui::imgui ALIAS imgui)
add_subdirectory(3rdparty/ImGuiFileDialog)

add_subdirectory(3rdparty/rtmidi)

add_subdirectory(AudioLib)
add_subdirectory(Interface)
add_subdirectory(app)
