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

add_executable(main0 main0.cc)
add_executable(main1 main1.cc)

target_link_libraries(main0
    opencv_world)
target_link_libraries(main1
    opencv_world)

install(TARGETS main0
    RUNTIME DESTINATION ${PROJECT_SOURCE_DIR})
install(TARGETS main1
    RUNTIME DESTINATION ${PROJECT_SOURCE_DIR})
