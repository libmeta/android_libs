cmake_minimum_required(VERSION 3.14)

project(ffmpeg)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-D__STDC_CONSTANT_MACROS)

if(ANDROID)    
    set(TAG 5.0)
    set( ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/v${TAG}_android_arm64-v8a)
    set( META_INCLUDES ${META_INCLUDES}         ${ROOT_DIR}/include             CACHE INTERNAL "includes list")
#    set( META_DIRECTORIES ${META_DIRECTORIES}   ${ROOT_DIR}/lib                 CACHE INTERNAL "directories list")
    set( META_DIRECTORIES ${META_DIRECTORIES}   ${CMAKE_CURRENT_BINARY_DIR}     CACHE INTERNAL "directories list")
    foreach(LIB
            libz.so
            libsrt.so
            libssl.so
            libcrypto.so
            libavcodec.so
            libavfilter.so
            libavformat.so
            libavutil.so
            libswresample.so
            libswscale.so
            )
        set( META_LIBS ${META_LIBS} ${LIB}  CACHE INTERNAL "libs list")
    endforeach()
    file(GLOB FFMPEG_SO ${ROOT_DIR}/lib/*.so)
    file(COPY ${FFMPEG_SO}       DESTINATION ${CMAKE_CURRENT_BINARY_DIR} FOLLOW_SYMLINK_CHAIN)
endif(ANDROID)

set( META_INCLUDES ${META_INCLUDES}     ${CMAKE_CURRENT_SOURCE_DIR}            CACHE INTERNAL "includes list")
set( META_INCLUDES ${META_INCLUDES}     ${CMAKE_CURRENT_SOURCE_DIR}/ffwrap     CACHE INTERNAL "includes list")
set( META_INCLUDES ${META_INCLUDES}     ${CMAKE_CURRENT_SOURCE_DIR}/srtwrap    CACHE INTERNAL "includes list")
