cmake_minimum_required(VERSION 3.0)
project(main)

add_definitions("-Wall -g")

add_executable(${PROJECT_NAME} main.cpp )

install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR})
