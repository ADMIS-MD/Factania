find_package(grit QUIET REQUIRED)

function(add_grit_step)
    add_custom_command(
        TARGET
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated.h
        COMMAND /path/to/your/custom_tool --input=${CMAKE_CURRENT_SOURCE_DIR}/input.file --output=${CMAKE_CURRENT_BINARY_DIR}/generated.h
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/input.file
        COMMENT "Running custom tool to generate header file"
    )

endfunction()