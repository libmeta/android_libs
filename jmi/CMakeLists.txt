cmake_minimum_required(VERSION 3.14)

project(jmi)

include(GitFetchContent)
include(MetaUtil)

git_fetch(${PROJECT_NAME} 500e8e76acda1e880340a441a4107a430ca0413b https://github.com/wang-bin/JMI.git)
add_meta_packet(${PROJECT_NAME})
add_meta_include(${CMAKE_CURRENT_SOURCE_DIR})
