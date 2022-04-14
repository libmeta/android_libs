## 添加第三方依赖包
cmake_minimum_required(VERSION 3.14)

project(json)

include(DownloadFetchContent)
include(MetaUtil)

download_fetch(${PROJECT_NAME} v3.10.5 https://github.com/nlohmann/json/releases/download  /include.zip)
add_meta_include(${${PROJECT_NAME}_INCLUDE}/single_include)
add_meta_include(${CMAKE_CURRENT_SOURCE_DIR})
