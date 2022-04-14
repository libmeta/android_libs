## 添加第三方依赖包
cmake_minimum_required(VERSION 3.14)
include(FetchContent)

macro(download_fetch DEP_NAME TAG URL PACKAGE)
    set(${DEP_NAME}_URL         ${URL})
    set(${DEP_NAME}_TAG         ${TAG})
    set(${DEP_NAME}_PACKAGE     ${${DEP_NAME}_TAG}${PACKAGE})

    FetchContent_Declare(
        ${DEP_NAME}
        URL         ${${DEP_NAME}_URL}/${${DEP_NAME}_PACKAGE}
        #    URL_HASH MD5=5588a7b18261c20068beabfb4f530b87
    )
FetchContent_MakeAvailable(${DEP_NAME})

set(${DEP_NAME}_ROOT  ${CMAKE_BINARY_DIR}/_deps/${DEP_NAME})
set(${DEP_NAME}_SRC             ${${DEP_NAME}_ROOT}-src)
set(${DEP_NAME}_BUILD           ${${DEP_NAME}_ROOT}-build)
set(${DEP_NAME}_INCLUDE         ${${DEP_NAME}_SRC})
set(${DEP_NAME}_DIRECTORY       ${${DEP_NAME}_BUILD})
set(${DEP_NAME}_LIB             ${DEP_NAME})

endmacro(downloadfetch)
