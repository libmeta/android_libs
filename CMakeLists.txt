cmake_minimum_required(VERSION 3.14)

project(meta_libs)

add_subdirectory(ffmpeg)

add_library(${PROJECT_NAME} INTERFACE)
target_link_libraries(
    ${PROJECT_NAME} INTERFACE
    ffmpeg

    )
