cmake_minimum_required(VERSION 3.0)
project(main)

set(TEST_FILES test.cpp)

add_library(test STATIC ${TEST_FILES})

include_directories(${PROJECT_SOURCE_DIR})

add_executable(${PROJECT_NAME} main.cpp )

target_link_libraries(${PROJECT_NAME}
    test
)

