cmake_minimum_required(VERSION 3.11.3)
project(Box2D VERSION 0.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DRELEASE")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

file(GLOB source_file
    "${CMAKE_CURRENT_SOURCE_DIR}/Box2D/Rope/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/Box2D/Rope/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Box2D/Dynamics/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/Box2D/Dynamics/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Box2D/Dynamics/Joints/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/Box2D/Dynamics/Joints/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Box2D/Dynamics/Contacts/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/Box2D/Dynamics/Contacts/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Box2D/Common/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/Box2D/Common/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Box2D/Collision/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/Box2D/Collision/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Box2D/Collision/Shapes/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/Box2D/Collision/Shapes/*.h"
)

add_library(Box2D STATIC
    ${source_file}
)

target_include_directories(Box2D PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})