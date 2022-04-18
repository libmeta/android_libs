macro(include_directory DIR_PATH)
    include_directories(${DIR_PATH})
    message("Include Directory: ${DIR_PATH}")
endmacro(include_directory)

function(include_directory_recursive directory)
    if (IS_DIRECTORY ${directory})
        include_directory(${directory})
        file(GLOB sub_dir_list RELATIVE ${directory} ${directory}/*)
        foreach (sub_dir ${sub_dir_list})
            if (IS_DIRECTORY ${directory}/${sub_dir})
                include_directory_recursive(${directory}/${sub_dir})
            endif (IS_DIRECTORY ${directory}/${sub_dir})
        endforeach (sub_dir ${sub_dir_list})
    endif (IS_DIRECTORY ${directory})
endfunction(include_directory_recursive directory)


macro(aux_include_directory directory directory_val)
    if (IS_DIRECTORY ${directory})
        list(APPEND ${directory_val} ${directory})
        file(GLOB sub_dir_list RELATIVE ${directory} ${directory}/*)
        foreach (sub_dir ${sub_dir_list})
            if (IS_DIRECTORY ${directory}/${sub_dir})
                aux_include_directory(${directory}/${sub_dir}  ${directory_val})
            endif (IS_DIRECTORY ${directory}/${sub_dir})
        endforeach (sub_dir ${sub_dir_list})
    endif (IS_DIRECTORY ${directory})
endmacro(aux_include_directory directory directory_val)



