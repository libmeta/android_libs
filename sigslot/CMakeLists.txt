cmake_minimum_required(VERSION 3.14)
project(sigslot)

include(GitFetchContent)
include(MetaUtil)

git_fetch(${PROJECT_NAME} v1.2.1 https://github.com/palacaze/sigslot.git)
add_meta_include(${CMAKE_BINARY_DIR}/_deps/${DEP_NAME}-src/include)
add_meta_include(${CMAKE_CURRENT_SOURCE_DIR})
