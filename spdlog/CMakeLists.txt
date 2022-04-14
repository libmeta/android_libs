cmake_minimum_required(VERSION 3.14)
project(spdlog)

include(DownloadFetchContent)
include(MetaUtil)

download_fetch(${PROJECT_NAME} v1.10.0 https://github.com/gabime/spdlog/archive/refs/tags .tar.gz)
add_meta_include(${${PROJECT_NAME}_INCLUDE}/include)
add_meta_directory(${${PROJECT_NAME}_DIRECTORY})
add_meta_lib(${${PROJECT_NAME}_LIB})
add_meta_include(${CMAKE_CURRENT_SOURCE_DIR})
