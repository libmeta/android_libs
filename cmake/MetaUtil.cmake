macro(add_meta_include INCLUDE)
    set( META_INCLUDES                  ${META_INCLUDES}           ${INCLUDE}                CACHE INTERNAL "includes list")
endmacro(add_meta_include)

macro(add_meta_directory DIRECTORY)
    set( META_DIRECTORIES               ${META_DIRECTORIES}        ${DIRECTORY}                 CACHE INTERNAL "directories list")
endmacro(add_meta_directory)

macro(add_meta_lib LIB)
    set( META_LIBS                      ${META_LIBS}                ${LIB}                    CACHE INTERNAL "libs list")
endmacro(add_meta_lib)

macro(add_meta_source SOURCE)
    set( META_SOURCES                   ${META_SOURCES}             ${SOURCE}                 CACHE INTERNAL "sources list")
endmacro(add_meta_source)

macro(add_meta_root INCLUDE DIRECTORY LIB)
    add_meta_include(${INCLUDE})
    add_meta_directory(${DIRECTORY})
    add_meta_lib(${LIB})
endmacro()

macro(add_meta_packet DEP_NAME )
    add_meta_root(${${DEP_NAME}_INCLUDE} ${${DEP_NAME}_DIRECTORY} ${${DEP_NAME}_LIB})
endmacro(add_meta_packet)

