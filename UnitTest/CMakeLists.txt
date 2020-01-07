cmake_minimum_required(VERSION 3.15.0)
project(wlEngine_UnitTest NONE)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DRELEASE")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/gtest/)

file(GLOB source_file
    "GameObject/*.cpp" "GameObject/*.hpp"
    "Component/*.cpp" "Component/*.hpp"
    "Component/Struct/*.cpp" "Component/Struct/*.hpp"
    "System/*.cpp" "System/*.hpp"
    "Graphics/*.cpp" "Graphics/*.hpp"
    "Memory/*.cpp" "Memory/*.hpp"
    "Event/*.cpp" "Event/*.hpp"
    "GameEditor/*.cpp" "GameEditor/*.hpp"
    "Manager/*.cpp" "Manager/*.hpp"
    "Utility/*.cpp" "Utility/*.hpp"
    "*.cpp" "*.hpp"
)

foreach(FILE ${source_file})
    get_filename_component(Dir ${FILE} DIRECTORY)
    get_filename_component(Exten ${FILE} EXT)

    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${Dir}")
    string(REPLACE "/" "\\" GROUP "${GROUP}")
    string(REPLACE "/" "\\" FILE "${FILE}")

    if ("${Exten}" STREQUAL ".cpp" OR "${Exten}" STREQUAL ".c")
       set(GROUP "Source Files${GROUP}")
    elseif("${Exten}" STREQUAL ".hpp" OR "${Exten}" STREQUAL ".h")
       set(GROUP "Header Files${GROUP}")
    endif()

    source_group("${GROUP}" FILES "${FILE}")
endforeach()

add_executable(wlEngine_UnitTest ${source_file})

target_include_directories(wlEngine_UnitTest
    PRIVATE "gtest/googletest/include"
    PRIVATE "gtest/googlemock/include"
)

target_link_libraries(wlEngine_UnitTest 
    gtest
    gmock
)

#Visual Studio Settings
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug$<$<CONFIG:Debug>:Debug>")