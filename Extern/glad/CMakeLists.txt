cmake_minimum_required(VERSION 3.11.3)
project(glad VERSION 0.1)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_library(glad STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/glad.c
)

target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)