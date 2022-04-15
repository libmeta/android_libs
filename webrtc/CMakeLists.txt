cmake_minimum_required(VERSION 3.14)
project(webrtc)

include(DownloadFetchContent)
include(MetaUtil)

if(ANDROID)
    set(${PROJECT_NAME}_PACKAGE /webrtc.android.tar.gz)
endif(ANDROID)

if(APPLE)
    set(${PROJECT_NAME}_PACKAGE  /webrtc.ios.tar.gz)
endif(APPLE)

if(WIN32)
    set(${PROJECT_NAME}_PACKAGE  /webrtc.windows_x86_64.zip)
endif()

download_fetch(${PROJECT_NAME} m100.4896.1.2 https://github.com/shiguredo-webrtc-build/webrtc-build/releases/download ${${PROJECT_NAME}_PACKAGE})
add_meta_include(${${PROJECT_NAME}_SRC}/include)
add_meta_lib(${PROJECT_NAME})
if(ANDROID)
    add_meta_directory(${${PROJECT_NAME}_SRC}/lib/${CMAKE_ANDROID_ARCH_ABI})
    add_meta_include(${${PROJECT_NAME}_SRC}/include/third_party/abseil-cpp)
    add_meta_include(${${PROJECT_NAME}_SRC}/include/third_party/boringssl/src/include)
    add_meta_include(${${PROJECT_NAME}_SRC}/include/third_party/libyuv/include)
endif(ANDROID)

add_meta_include(${CMAKE_CURRENT_SOURCE_DIR})
