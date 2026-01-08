cmake_minimum_required(VERSION 3.16)

project(Factania LANGUAGES C)

include(scripts/cmake/git.cmake)

add_executable(factoryds 
	src/main.c
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

if(UNIX)
    set(NDS_BUILD_SCRIPT "build.sh")
elseif(WIN32)
    set(NDS_BUILD_SCRIPT "build.bat")
else()

endif()

add_custom_target(Build_NDS
    COMMAND ${NDS_BUILD_SCRIPT}
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/scripts)