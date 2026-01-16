cmake_minimum_required(VERSION 3.16)

# Force the compiler to work no matter what
set(CMAKE_CXX_COMPILER_WORKS 1)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True) 

project(Factania LANGUAGES C CXX)

include(scripts/cmake/git.cmake)
string(TIMESTAMP BUILD_DATE "%Y-%m-%d")

configure_file(src/generated_defines.h.in generated/generated_defines.h)

set(SOURCE_FILES
  src/main.c
  src/FactaniaMain.cpp
  src/Engine.cpp
  src/RenderSystem.cpp
  src/debug_menu/debug_menu.cpp
  src/debug_menu/memory_debug_info.cpp
  src/debug_menu/build_details_menu.cpp
)

set(HEADER_FILES
src/FactaniaMain.h
  src/Engine.h
  src/System.h
  src/RenderSystem.h
  src/debug_menu/debug_menu.h
  src/debug_menu/memory_debug_info.h
  src/debug_menu/build_details_menu.h
  generated/generated_defines.h
)

add_executable(Factania ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(Factania PRIVATE sdk/libs/libnds/include src/)
target_include_directories(Factania PRIVATE ${CMAKE_BINARY_DIR}/generated)

if(NINTENDO_DS)
    nds_create_rom(factoryds
        NAME "FACTANIA"
        SUBTITLE "Build ${GIT_COMMIT_HASH_SHORT}"
        AUTHOR "ADMIS Manufacturing Division"
        NITROFS fs
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

