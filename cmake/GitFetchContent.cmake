## 添加第三方依赖包
cmake_minimum_required(VERSION 3.14)
include(FetchContent)

macro(git_fetch DEP_NAME TAG URL)
    FetchContent_Declare(
        ${DEP_NAME}
        GIT_REPOSITORY    ${URL}
        GIT_TAG           ${TAG}
    )

FetchContent_MakeAvailable(${DEP_NAME})

set(${DEP_NAME}_ROOT            ${CMAKE_BINARY_DIR}/_deps/${DEP_NAME})
set(${DEP_NAME}_SRC             ${${DEP_NAME}_ROOT}-src)
set(${DEP_NAME}_BUILD           ${${DEP_NAME}_ROOT}-build)
set(${DEP_NAME}_INCLUDE         ${${DEP_NAME}_SRC})
set(${DEP_NAME}_DIRECTORY       ${${DEP_NAME}_BUILD})
set(${DEP_NAME}_LIB             ${DEP_NAME})
endmacro(gitfetch)
