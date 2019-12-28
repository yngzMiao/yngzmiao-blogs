cmake_minimum_required(VERSION 3.0)
project(main)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
add_definitions("-Wall -g")

set(GOOGLE_PROTOBUF_DIR ${PROJECT_SOURCE_DIR}/protobuf)
set(PROTO_PB_DIR ${PROJECT_SOURCE_DIR}/proto_pb2)

add_subdirectory(proto_pb2)

include_directories(
  ${PROTO_PB_DIR}
)

add_executable(${PROJECT_NAME} main.cc )

target_link_libraries(${PROJECT_NAME}
  person_pb2
)

install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR})
