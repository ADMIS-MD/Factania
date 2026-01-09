cmake_minimum_required(VERSION 3.16)

project(Factania LANGUAGES C)

include(scripts/cmake/git.cmake)

add_executable(factoryds 
	src/main.c
    src/batcher.c
        src/factory_obj.c
        src/factory_obj.h
        src/chunk.c
        src/rng.h
)

target_include_directories(factoryds PRIVATE sdk/libs/libnds/include)

if(NINTENDO_DS)
    nds_create_rom(factoryds
        NAME "FACTANIA"
        SUBTITLE "Build ${GIT_COMMIT_HASH_SHORT}"
        AUTHOR "ADMIS Manufacturing Division"
    )
else()
    target_compile_definitions(factoryds PUBLIC ARM9)
endif()

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
    COMMAND scripts/melonds/${MELONDS_PATH} build/factoryds.nds
    DEPENDS Build_NDS
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Melon_NDS)
