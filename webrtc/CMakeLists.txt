cmake_minimum_required(VERSION 3.14)
project(webrtc)

include(DownloadFetchContent)
include(MetaUtil)

if(ANDROID)
    set(DEP_NAME ${PROJECT_NAME}_android)
    set(${DEP_NAME}_PACKAGE  /webrtc-android.zip)
endif(ANDROID)

download_fetch(${DEP_NAME} M96 https://github.com/Unity-Technologies/com.unity.webrtc/releases/download/ ${${DEP_NAME}_PACKAGE})
add_meta_include(${${DEP_NAME}_SRC}/include)
add_meta_lib(${PROJECT_NAME})

if(ANDROID)
    add_meta_directory(${${DEP_NAME}_SRC}/lib/${CMAKE_ANDROID_ARCH})
    add_meta_include(${${DEP_NAME}_SRC}/include/third_party/abseil-cpp)
    #    add_meta_include(${${PROJECT_NAME}_SRC}/include/third_party/boringssl/src/include)
    add_meta_include(${${DEP_NAME}_SRC}/include/third_party/jsoncpp/source/include)
    add_meta_include(${${DEP_NAME}_SRC}//include/third_party/jsoncpp/generated)
    add_meta_include(${${DEP_NAME}_SRC}/include/third_party/libyuv/include)
endif(ANDROID)

add_meta_include(${CMAKE_CURRENT_SOURCE_DIR})
