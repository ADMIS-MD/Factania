find_package(Git QUIET)
if (GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    # Get the short hash
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_HASH_SHORT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_BRANCH_NAME
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-list --count HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_BRANCH_COMMIT_NUMBER
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    message(STATUS "Git commit hash: ${GIT_COMMIT_HASH}")
    message(STATUS "Git short commit hash: ${GIT_COMMIT_HASH_SHORT}")
else()
    message(WARNING "Git not found. Cannot determine commit hash.")
    set(GIT_COMMIT_HASH "unknown" CACHE INTERNAL "")
    set(GIT_COMMIT_HASH_SHORT "unknown" CACHE INTERNAL "")
endif()

