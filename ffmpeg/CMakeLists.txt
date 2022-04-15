cmake_minimum_required(VERSION 3.14)
project(ffmpeg)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_CXX_FLAGS} -D__STDC_CONSTANT_MACROS ")
add_compile_definitions(__STDC_CONSTANT_MACROS)

include(OptionDefinition)
include(MetaUtil)

if(ANDROID)
    set(DEP_NAME ffmpeg_android)
    set(${DEP_NAME}_PACKAGE /ffmpeg-5.0.1-android.tar.xz)
    download_fetch(${DEP_NAME} v1.0 https://github.com/libmeta/ffmpeg_build/releases/download  ${${DEP_NAME}_PACKAGE})
    set( ROOT_DIR ${${DEP_NAME}_SRC})
    add_meta_include(${ROOT_DIR}/include)
    add_meta_directory(${ROOT_DIR}/lib)
    #    add_meta_directory(${CMAKE_CURRENT_BINARY_DIR})
    foreach(LIB
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
    add_meta_lib(${LIB})
endforeach()

file(GLOB FFMPEG_SO     ${ROOT_DIR}/lib/*.so)
file(COPY ${FFMPEG_SO}   DESTINATION ${CMAKE_CURRENT_BINARY_DIR} FOLLOW_SYMLINK_CHAIN)
endif(ANDROID)

add_meta_include(${CMAKE_CURRENT_SOURCE_DIR})
add_meta_include(${CMAKE_CURRENT_SOURCE_DIR}/ffwrap)
add_meta_include(${CMAKE_CURRENT_SOURCE_DIR}/srtwrap)

