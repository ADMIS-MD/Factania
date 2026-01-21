cmake_minimum_required(VERSION 3.16)

# Force the compiler to work no matter what
set(CMAKE_CXX_COMPILER_WORKS 1)

project(Factania LANGUAGES C CXX)

include(scripts/cmake/git.cmake)
string(TIMESTAMP BUILD_DATE "%Y-%m-%d")

configure_file(src/generated_defines.h.in generated/generated_defines.h)

set(SOURCE_FILES
  src/main.c
  src/factania_main.cpp
  src/engine.cpp
  src/debug_menu/debug_menu.cpp
  src/debug_menu/memory_debug_info.cpp
  src/debug_menu/build_details_menu.cpp
  src/debug_menu/save_debug_menu.cpp
  src/save.cpp
)

set(HEADER_FILES
  src/engine.h
  src/factania_main.h
  src/System.h
  src/debug_menu/debug_menu.h
  src/debug_menu/memory_debug_info.h
  src/debug_menu/build_details_menu.h
  generated/generated_defines.h
  src/save.h
)

add_executable(Factania ${SOURCE_FILES} ${HEADER_FILES})

# Include all sdk header directories
target_include_directories(Factania PRIVATE
        src/
        sdk/libs/libnds/include
        sdk/libs/dswifi/include
        sdk/libs/libteak/include
        sdk/libs/libxm7/include
        sdk/libs/maxmod/include)

target_include_directories(Factania PRIVATE ${CMAKE_BINARY_DIR}/generated)

if(NINTENDO_DS)
    target_link_libraries(Factania PRIVATE
        "-Wl,--start-group"
        nds9
        c
        "-Wl,--end-group"
    )

    nds_create_rom(Factania
        NAME "FACTANIA"
        SUBTITLE "Build ${GIT_COMMIT_HASH_SHORT}"
        AUTHOR "ADMIS Manufacturing Division"
        NITROFS fs
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
    COMMAND scripts/melonds/${MELONDS_PATH} build/Factania.nds
    DEPENDS Build_NDS
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Melon_NDS)

