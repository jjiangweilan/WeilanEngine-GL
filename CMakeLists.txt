cmake_minimum_required(VERSION 3.11.3)
project(wlEngine VERSION 0.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DRELEASE")

#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/Box2D/)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/SDL2/)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/freetype/)

add_compile_definitions(ROOT_DIR=std::string\(\"${CMAKE_CURRENT_SOURCE_DIR}\"\))

set(SDL2_INCLUDE_DIR third_party/SDL2/include)
set(glm_INCLUDE_DIR third_party/glm/) #header only
set(json_INCLUDE_DIR third_party/json/include) #header only
set(stb_image_INCLUDE_DIR third_party/stb/include) #header only
set(glad_INCLUDE_DIR third_party/glad/include)
set(assimp_INCLUDE_DIR third_party/assimp/include)
set(dirent_INCLUDE_DIR third_party/dirent/include)
set(freetype_INCLUDE_DIR third_party/freetype/include)
set(SDL2_mixer_INCLUDE_DIR third_party/SDL2_mixer/include)
set(lua_INCLUDE_DIR third_party/lua/src)

find_package(OpenGL REQUIRED)
find_library(assimp_LIBRARY assimp HINTS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/assimp/lib/windows)
find_library(SDL2_mixer_LIBRARY SDL2_mixer HINTS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/SDL2_mixer/lib)
find_library(SDL2_LIBRARY SDL2 HINTS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/SDL2/lib/x64)

SET(INCLUDE_DIR 
    ${glm_INCLUDE_DIR}
    ${json_INCLUDE_DIR}
    ${stb_image_INCLUDE_DIR}
    ${glad_INCLUDE_DIR}
    ${assimp_INCLUDE_DIR} 
    ${dirent_INCLUDE_DIR}
    ${SDL2_INCLUDE_DIR}
    ${freetype_INCLUDE_DIR}
    ${lua_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${SDL2_mixer_INCLUDE_DIR}
)

include_directories(
    ${INCLUDE_DIR}
)

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
    "third_party/imgui/*.cpp"
    "third_party/lua/src/*.c"
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

add_library(wlEngine STATIC
	${source_file})

TARGET_LINK_LIBRARIES(wlEngine
    #Box2D
    glad
    freetype
    ${SDL2_LIBRARY}
    ${SDL2_mixer_LIBRARY}
    ${OPENGL_LIBRARIES}
    ${assimp_LIBRARY}
)

TARGET_INCLUDE_DIRECTORIES(wlEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${INCLUDE_DIR}
)