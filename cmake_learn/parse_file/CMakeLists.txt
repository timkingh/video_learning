cmake_minimum_required (VERSION 2.8...3.27)

project(TEST)

message(STATUS "This is BINARY dir " ${PROJECT_BINARY_DIR})
message(STATUS "This is SOURCE dir " ${PROJECT_SOURCE_DIR})

add_subdirectory(lib)
add_subdirectory(src)
