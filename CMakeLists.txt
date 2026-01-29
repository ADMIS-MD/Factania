cmake_minimum_required(VERSION 3.16)

# Force the compiler to work no matter what
set(CMAKE_CXX_COMPILER_WORKS 1)
set(CMAKE_COLOR_DIAGNOSTICS 1)



project(Factania LANGUAGES C CXX)


# Generate h files with infomration
include(scripts/cmake/git.cmake)
string(TIMESTAMP BUILD_DATE "%Y-%m-%d")
configure_file(src/generated_defines.h.in generated/generated_defines.h)

set(SOURCE_FILES
    src/main.c
    src/factania_main.cpp
    src/Engine.cpp
    src/RenderSystem.cpp
    src/Camera.cpp
    src/debug_menu/debug_menu.cpp
    src/debug_menu/memory_debug_info.cpp
    src/debug_menu/build_details_menu.cpp
    src/debug_menu/save_debug_menu.cpp
    src/Save.cpp
    src/chunk.cpp
    src/Math.cpp
    src/Player.cpp
    src/building.cpp
)

set(HEADER_FILES
    src/Engine.h
    src/factania_main.h
    src/System.h
    src/RenderSystem.h
    src/Camera.h
    src/debug_menu/debug_menu.h
    src/debug_menu/memory_debug_info.h
    src/debug_menu/build_details_menu.h
    generated/generated_defines.h
    src/Save.h
    src/chunk.hpp
    src/Transform.cpp
    src/Transform.h
    src/Player.h
    src/Sprite.h
    src/building.h
    src/item.h
)

# Create fake executable target
add_executable(Factania ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(Factania PRIVATE
        # Include all sdk header directories
        sdk/libs/libnds/include
        sdk/libs/dswifi/include
        sdk/libs/libteak/include
        sdk/libs/libxm7/include
        sdk/libs/maxmod/include

        # Etc
        src/
        ${CMAKE_BINARY_DIR}/generated
        extern
)


if(NINTENDO_DS)
    target_link_libraries(Factania PRIVATE
        "-Wl,--start-group"
        nds9
        c
        "-Wl,--end-group"
    )

    # Grit target - only works on nds toolchain
    file(GLOB_RECURSE GRIT_SRCS assets/*.png)
    set(GRIT_OUT_FILES)

    foreach(file_path ${GRIT_SRCS})
        get_filename_component(filename_wle ${file_path} NAME_WLE)
        list(APPEND GRIT_OUT_FILES generated/${filename_wle}.c)

        # Test if we need to remake textures
        if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/generated/${filename_wle}.c")
            if("${CMAKE_CURRENT_BINARY_DIR}/generated/${filename_wle}.c" IS_NEWER_THAN ${file_path})
                continue()
            endif()
        endif()

        execute_process(
            COMMAND ${BLOCKSDS}/tools/grit/grit ${file_path} -ftc -W1 -o${CMAKE_CURRENT_BINARY_DIR}/generated/${filename_wle}
        )
    endforeach()

    add_library(Grit
        OBJECT ${GRIT_OUT_FILES}
    )
    add_dependencies(Factania Grit)
    target_link_libraries(Factania PRIVATE Grit)

    # NDS rom tool
    nds_create_rom(Factania
        NAME "FACTANIA"
        SUBTITLE "Build ${GIT_COMMIT_HASH_SHORT}"
        AUTHOR "ADMIS Manufacturing Division"
        NITROFS fs
    )
else()
    # Specify arm9 if we aren't ds for compat
    target_compile_definitions(Factania PUBLIC ARM9)
endif()


# Setup extra nds dspecific build and run "targets"
find_program(MELONDS_PATH "melonDS")
if(NOT MELONDS_PATH)
    if(UNIX)
        set(MELONDS_PATH "./melonDS")
    elseif(WIN32)
        set(MELONDS_PATH "melonDS.exe")
    endif()
endif()

if(UNIX)
    set(NDS_BUILD_SCRIPT "./build.sh")
elseif(WIN32)
    set(NDS_BUILD_SCRIPT "build.bat")
endif()

add_custom_target(Build_NDS
    COMMAND ${NDS_BUILD_SCRIPT}
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/scripts)

add_custom_target(Melon_NDS
    COMMAND scripts/melonds/${MELONDS_PATH} build/Factania.nds
    DEPENDS Build_NDS
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Melon_NDS)

