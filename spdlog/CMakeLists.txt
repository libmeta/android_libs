## 添加第三方依赖包
#include(FetchContent)
#cmake_minimum_required(VERSION 3.14)

#set(DEP_NAME  spdlog)
#set(${DEP_NAME}_GIT_TAG  v1.9.2)  # 指定版本
#set(${DEP_NAME}_GIT_URL  https://github.com/gabime/spdlog.git)  # 指定git仓库地址

#FetchContent_Declare(
#  ${DEP_NAME}
#  GIT_REPOSITORY    ${${DEP_NAME}_GIT_URL}
#  GIT_TAG           ${${DEP_NAME}_GIT_TAG}
#)

#FetchContent_MakeAvailable(${DEP_NAME})
#set( META_INCLUDES ${META_INCLUDES}  ${CMAKE_BINARY_DIR}/_deps/${DEP_NAME}-src/include  CACHE INTERNAL "includes list")

#cmake_minimum_required(VERSION 3.14)
#project(json)
#set(TAG 1.9.2)
#set(META_INCLUDES ${META_INCLUDES}  ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/spdlog-${TAG}/include  CACHE INTERNAL "includes list")
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/spdlog-${TAG}/include)


cmake_minimum_required(VERSION 3.14)

include(FetchContent)
set(DEP_NAME  spdlog)

set(${DEP_NAME}_URL         https://github.com/gabime/spdlog/archive/refs/tags)
set(${DEP_NAME}_TAG         v1.10.0)
set(${DEP_NAME}_PACKAGE     ${${DEP_NAME}_TAG}.tar.gz)

FetchContent_Declare(
    ${DEP_NAME}
    URL         ${${DEP_NAME}_URL}/${${DEP_NAME}_PACKAGE}
    #    URL_HASH MD5=5588a7b18261c20068beabfb4f530b87
)

FetchContent_MakeAvailable(${DEP_NAME})
set( META_INCLUDES  ${META_INCLUDES}        ${CMAKE_CURRENT_SOURCE_DIR}                         CACHE INTERNAL "includes list")
set( META_INCLUDES  ${META_INCLUDES}        ${CMAKE_BINARY_DIR}/_deps/${DEP_NAME}-src/include   CACHE INTERNAL "includes list")
