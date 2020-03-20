cmake_minimum_required(VERSION 3.0)
project(main)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
add_definitions("-Wall -g")

set(OPENCV_DIR ${PROJECT_SOURCE_DIR}/opencv)

include_directories(
    ${OPENCV_DIR}/include
)

link_directories(
    ${OPENCV_DIR}/lib
)

add_executable(${PROJECT_NAME} main.cc)

target_link_libraries(${PROJECT_NAME}
    opencv_world)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${PROJECT_SOURCE_DIR})
