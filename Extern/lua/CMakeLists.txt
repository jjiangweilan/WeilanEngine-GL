cmake_minimum_required(VERSION 3.15.0)
project(lua)

add_library(lua)

target_include_directories(lua
    INTERFACE
    Include/
    PRIVATE
    Include/lua
)

add_subdirectory(Source/)
