cmake_minimum_required(VERSION 3.16)

project(Factania LANGUAGES C)

include(scripts/cmake/git.cmake)

add_executable(factoryds 
	src/main.c
)

nds_create_rom(factoryds
	NAME "FACTANIA"
	SUBTITLE "Build ${GIT_COMMIT_HASH_SHORT}"
	AUTHOR "ADMIS Manufacturing Division"
)
