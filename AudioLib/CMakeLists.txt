cmake_minimum_required(VERSION 3.20)
project(AudioLib)

file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_library(${PROJECT_NAME} STATIC ${SOURCES})
add_library(AudioLib::library ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)


target_link_libraries(${PROJECT_NAME} PUBLIC rtmidi)
