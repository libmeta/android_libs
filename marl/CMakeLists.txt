cmake_minimum_required(VERSION 3.14)
project(marl)

include(GitFetchContent)
include(MetaUtil)

git_fetch(${PROJECT_NAME} 9929747c9ba6354691dbacaf14f9b35433871e5b https://github.com/google/marl.git)
add_meta_packet(${PROJECT_NAME})
add_meta_include(${CMAKE_CURRENT_SOURCE_DIR})
