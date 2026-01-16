cmake_minimum_required(VERSION 3.16)

# Force the compiler to work no matter what
set(CMAKE_CXX_COMPILER_WORKS 1)

project(Factania LANGUAGES C CXX)

include(scripts/cmake/git.cmake)

set(SOURCE_FILES
  src/main.c
  src/engine.cpp
  src/factania_main.cpp
)

set(HEADER_FILES
  src/engine.h
  src/factania_main.h
  src/system.h
)

add_executable(Factania ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(Factania PRIVATE sdk/libs/libnds/include src/)

if(NINTENDO_DS)
    nds_create_rom(Factania
        NAME "FACTANIA"
        SUBTITLE "Build ${GIT_COMMIT_HASH_SHORT}"
        AUTHOR "ADMIS Manufacturing Division"
    )
else()
    target_compile_definitions(Factania PUBLIC ARM9)
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
    COMMAND scripts/melonds/${MELONDS_PATH} build/factania.nds
    DEPENDS Build_NDS
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Melon_NDS)

