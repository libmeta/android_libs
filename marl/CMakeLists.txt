
# 添加第三方依赖包
include(FetchContent)
cmake_minimum_required(VERSION 3.14)

set(DEP_NAME  marl)
set(${DEP_NAME}_GIT_TAG  9929747c9ba6354691dbacaf14f9b35433871e5b)  # 指定版本
set(${DEP_NAME}_GIT_URL  https://github.com/google/marl.git)  # 指定git仓库地址

FetchContent_Declare(
    ${DEP_NAME}
    GIT_REPOSITORY    ${${DEP_NAME}_GIT_URL}
    GIT_TAG           ${${DEP_NAME}_GIT_TAG}
    )

#    include_directories(${CMAKE_BINARY_DIR}/_deps/${DEP_NAME}-src)
FetchContent_MakeAvailable(${DEP_NAME})
set( META_INCLUDES          ${META_INCLUDES}        ${CMAKE_BINARY_DIR}/_deps/${DEP_NAME}-src       CACHE INTERNAL "includes list")
set( META_DIRECTORIES       ${META_DIRECTORIES}     ${CMAKE_BINARY_DIR}/_deps/${DEP_NAME}-build     CACHE INTERNAL "directories list")
set( META_LIBS              ${META_LIBS}            ${DEP_NAME}                                     CACHE INTERNAL "libs list")


