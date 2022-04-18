cmake_minimum_required(VERSION 3.14)

project(meta_libs)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/meta_libs/cmake")
include(InitSystemName)
include(OptionDefinition)

set(META_INCLUDES       "" CACHE INTERNAL "includes list")
set(META_LIBS           "" CACHE INTERNAL "libraries list")
set(META_DIRECTORIES    "" CACHE INTERNAL "directories list")
set(META_SOURCES        "" CACHE INTERNAL "sources list")

add_subdirectory(spdlog)
add_subdirectory(utils)
add_subdirectory(json)

if(ANDROID)
    add_subdirectory(jmi)
endif(ANDROID)

if(HAVE_SIGSLOT)
    add_subdirectory(sigslot)
endif(HAVE_SIGSLOT)

if(HAVE_MARL)
    add_subdirectory(marl)
endif(HAVE_MARL)

if(HAVE_FFMPEG)
    add_subdirectory(ffmpeg)
endif(HAVE_FFMPEG)

if(HAVE_WEBRTC)
    add_subdirectory(webrtc)
endif(HAVE_WEBRTC)

file(GLOB META_SO    ${CMAKE_CURRENT_BINARY_DIR}/*/*.so)
file(COPY ${META_SO} DESTINATION ${CMAKE_CURRENT_BINARY_DIR} FOLLOW_SYMLINK_CHAIN)
message("file(COPY ${REMOTE_API_SO} DESTINATION      ${_ANDROID_LIBRARY_DIR} FOLLOW_SYMLINK_CHAIN)")
