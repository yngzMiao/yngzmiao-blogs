cmake_minimum_required(VERSION 3.0)
project(main)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -pthread")
add_definitions("-Wall -g")

add_executable(${PROJECT_NAME} main.cc)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${PROJECT_SOURCE_DIR})
