cmake_minimum_required(VERSION 3.14)

project(meta_libs)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/meta_libs/cmake")

set(META_INCLUDES       "" CACHE INTERNAL "includes list")
set(META_LIBS           "" CACHE INTERNAL "libraries list")
set(META_DIRECTORIES    "" CACHE INTERNAL "directories list")
set(META_SOURCES        "" CACHE INTERNAL "sources list")

add_subdirectory(webrtc)
add_subdirectory(jmi)
add_subdirectory(sigslot)
add_subdirectory(marl)
add_subdirectory(ffmpeg)
add_subdirectory(json)
add_subdirectory(spdlog)
add_subdirectory(utils)

file(GLOB META_SO    ${CMAKE_CURRENT_BINARY_DIR}/*/*.so)
file(COPY ${META_SO} DESTINATION ${CMAKE_CURRENT_BINARY_DIR} FOLLOW_SYMLINK_CHAIN)
message("file(COPY ${REMOTE_API_SO} DESTINATION      ${_ANDROID_LIBRARY_DIR} FOLLOW_SYMLINK_CHAIN)")
