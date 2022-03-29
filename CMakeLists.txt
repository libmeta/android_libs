cmake_minimum_required(VERSION 3.14)

project(meta_libs)

#_________________________WARNINGS__________________________________________
#TODO set it for all targets individually or remove for WARNING HELL
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options("-w")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options("-w")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    cmake_policy(SET CMP0092 NEW) #cmake disable std MSVC warnings in CMAKE_C_FLAGS
    #CMAKE_DEPFILE_FLAGS_C var in windows contains only /showIncludes and produce include tree
    set(CMAKE_DEPFILE_FLAGS_C "") #erase it
    add_compile_options("/w")
endif()

set(META_INCLUDES       "" CACHE INTERNAL "includes list")
set(META_LIBS           "" CACHE INTERNAL "libraries list")
set(META_DIRECTORIES    "" CACHE INTERNAL "directories list")
add_subdirectory(ffmpeg)
file(GLOB META_SO    ${CMAKE_CURRENT_BINARY_DIR}/*/*.so)
file(COPY ${META_SO} DESTINATION ${CMAKE_CURRENT_BINARY_DIR} FOLLOW_SYMLINK_CHAIN)
message("file(COPY ${REMOTE_API_SO} DESTINATION      ${_ANDROID_LIBRARY_DIR} FOLLOW_SYMLINK_CHAIN)")
