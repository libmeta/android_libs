#_________________________WARNINGS__________________________________________
#TODO set it for all targets individually or remove for WARNING HELL
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options("-w")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options("-w")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    cmake_policy(SET CMP0092 NEW) #cmake disable std MSVC warnings in CMAKE_C_FLAGS
    #CMAKE_DEPFILE_FLAGS_C var in windows contains only /showIncludes and produce include tree
    set(CMAKE_DEPFILE_FLAGS_C "") #erase it
    add_compile_options("/w")
endif()
