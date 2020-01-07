cmake_minimum_required(VERSION 3.0)
include(cmake/utils.cmake)
project(main)

string(TIMESTAMP BUILD_TIMESTAMP "%Y-%m-%d %H:%M:%S")
message("Build timestamp is ${BUILD_TIMESTAMP}")

set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 1)
message("Version is ${VERSION_MAJOR} ${VERSION_MINOR} ${VERSION_PATCH}")

set(GIT_HASH "")
get_git_hash(GIT_HASH)
message("Git hash is ${GIT_HASH}")

set(GIT_BRANCH "")
get_git_branch(GIT_BRANCH)
message("Git branch is ${GIT_BRANCH}")

configure_file (
    "${PROJECT_SOURCE_DIR}/include/utils.h.in"
    "${PROJECT_SOURCE_DIR}/include/utils.h"
)

add_executable(${PROJECT_NAME} src/main.cc)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${PROJECT_SOURCE_DIR})

