cmake_minimum_required(VERSION 3.14)

project(ffmpeg)

if(ANDROID)    
    add_library(${META_LIBS_NAME} INTERFACE)
    set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android_arm64-v8a)

    target_include_directories(${META_LIBS_NAME} INTERFACE
        ${ROOT_DIR}/include)
    target_link_libraries(
        ${META_LIBS_NAME} INTERFACE
        ${ROOT_DIR}/lib/libsrt.so
        ${ROOT_DIR}/lib/libssl.so
        ${ROOT_DIR}/lib/libcrypto.so
        ${ROOT_DIR}/lib/libavcodec.so
        ${ROOT_DIR}/lib/libavfilter.so
        ${ROOT_DIR}/lib/libavformat.so
        ${ROOT_DIR}/lib/libavutil.so
        ${ROOT_DIR}/lib/libswresample.so
        ${ROOT_DIR}/lib/libswscale.so
#        ${ROOT_DIR}/lib/libpostproc.so
        )

    file(COPY ${ROOT_DIR}/lib/libsrt.so         DESTINATION ${CMAKE_BINARY_DIR} FOLLOW_SYMLINK_CHAIN)
    file(COPY ${ROOT_DIR}/lib/libssl.so         DESTINATION ${CMAKE_BINARY_DIR} FOLLOW_SYMLINK_CHAIN)
    file(COPY ${ROOT_DIR}/lib/libcrypto.so      DESTINATION ${CMAKE_BINARY_DIR} FOLLOW_SYMLINK_CHAIN)
    file(COPY ${ROOT_DIR}/lib/libavcodec.so     DESTINATION ${CMAKE_BINARY_DIR} FOLLOW_SYMLINK_CHAIN)
    file(COPY ${ROOT_DIR}/lib/libavfilter.so    DESTINATION ${CMAKE_BINARY_DIR} FOLLOW_SYMLINK_CHAIN)
    file(COPY ${ROOT_DIR}/lib/libavformat.so    DESTINATION ${CMAKE_BINARY_DIR} FOLLOW_SYMLINK_CHAIN)
    file(COPY ${ROOT_DIR}/lib/libavutil.so      DESTINATION ${CMAKE_BINARY_DIR} FOLLOW_SYMLINK_CHAIN)
    file(COPY ${ROOT_DIR}/lib/libswresample.so  DESTINATION ${CMAKE_BINARY_DIR} FOLLOW_SYMLINK_CHAIN)
    file(COPY ${ROOT_DIR}/lib/libswscale.so     DESTINATION ${CMAKE_BINARY_DIR} FOLLOW_SYMLINK_CHAIN)

endif(ANDROID)
