cmake_minimum_required(VERSION 3.11.3)
project(demo VERSION 0.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DRELEASE")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/wlEngine)

include_directories(
    ${INCLUDE_DIR}
)

file(GLOB source_file
    main.cpp
    "Script/*.cpp"
    "Script/*.hpp"
    "Script/Monster/*.hpp"
    "Script/Monster/*.cpp"
    "Script/Player/*.cpp"
    "Script/Player/*.cpp"
    "Script/AiScript/*.cpp"
    "Script/AiScript/*.hpp"
    "System/*.hpp"
    "System/*.cpp"
    "Component/*.hpp"
    "Component/*.cpp"
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

add_executable(demo ${source_file})

TARGET_LINK_LIBRARIES(demo
    wlEngine
)

TARGET_INCLUDE_DIRECTORIES(demo PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${INCLUDE_DIR}
)

file(GLOB SDL2_mixer_lib
    ${CMAKE_CURRENT_SOURCE_DIR}/wlEngine/third_party/SDL2_mixer/lib/*.dll
)

foreach(FILE ${SDL2_mixer_lib})
        configure_file(${FILE} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
endforeach()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/wlEngine/third_party/assimp/lib/windows/assimp-vc140-mt.dll ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/wlEngine/third_party/SDL2/lib/x64/SDL2.dll ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)