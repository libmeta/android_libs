cmake_minimum_required(VERSION 3.14)
include(FetchContent)

set(DEP_NAME  webrtc)
set(${DEP_NAME}_URL     https://github.com/shiguredo-webrtc-build/webrtc-build/releases/download)
set(${DEP_NAME}_TAG     m100.4896.1.2)

if(ANDROID)
    set(${DEP_NAME}_PACKAGE ${${DEP_NAME}_TAG}/webrtc.android.tar.gz)
endif(ANDROID)

if(APPLE)
    set(${DEP_NAME}_PACKAGE  ${${DEP_NAME}_TAG}/webrtc.ios.tar.gz)
endif(APPLE)

if(WIN32)
    set(${DEP_NAME}_PACKAGE  ${${DEP_NAME}_TAG}/webrtc.windows_x86_64.zip)
endif()

FetchContent_Declare(
    ${DEP_NAME}
    URL         ${${DEP_NAME}_URL}/${${DEP_NAME}_PACKAGE}
    #    URL_HASH MD5=5588a7b18261c20068beabfb4f530b87
)

FetchContent_MakeAvailable(${DEP_NAME})
set( META_INCLUDES          ${META_INCLUDES}        ${CMAKE_CURRENT_SOURCE_DIR}                                     CACHE INTERNAL "includes list")
set( META_INCLUDES          ${META_INCLUDES}        ${CMAKE_BINARY_DIR}/_deps/${DEP_NAME}-src/include               CACHE INTERNAL "includes list")

if(ANDROID)
set( META_DIRECTORIES       ${META_DIRECTORIES}     ${CMAKE_BINARY_DIR}/_deps/${DEP_NAME}-src/lib/${CMAKE_ANDROID_ARCH_ABI}   CACHE INTERNAL "directories list")
endif(ANDROID)

set( META_LIBS              ${META_LIBS}            ${DEP_NAME}                                                     CACHE INTERNAL "libs list")
