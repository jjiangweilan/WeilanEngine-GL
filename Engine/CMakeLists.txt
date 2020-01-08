cmake_minimum_required(VERSION 3.15.0)
project(wlEngine NONE)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DRELEASE")

#Extern
add_subdirectory(Extern/glad/)
add_subdirectory(Extern/freetype/)

add_compile_definitions(ROOT_DIR=std::string\(\"${CMAKE_CURRENT_SOURCE_DIR}\"\))

find_package(OpenGL REQUIRED)
find_library(assimp_LIBRARY assimp HINTS ${CMAKE_CURRENT_SOURCE_DIR}/Extern/assimp/lib/windows)
find_library(SDL2_mixer_LIBRARY SDL2_mixer HINTS ${CMAKE_CURRENT_SOURCE_DIR}/Extern/SDL2_mixer/lib)
find_library(SDL2_LIBRARY SDL2 HINTS ${CMAKE_CURRENT_SOURCE_DIR}/Extern/SDL2/lib/x64)

add_library(wlEngine)
target_include_directories(wlEngine
    PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)
add_subdirectory(Header/)
add_subdirectory(Source/)

target_include_directories(wlEngine
    PUBLIC 
    Extern/glm/
    Extern/json/include/
    Extern/dirent/include/
    Extern/SDL2/include/
    PRIVATE 
    Extern/stb/include/
    Extern/glad/include/
    Extern/assimp/include/
    Extern/freetype/include/
    Extern/lua/src/
    Extern/SDL2_mixer/include/
)

TARGET_LINK_LIBRARIES(wlEngine
    glad
    freetype
    ${SDL2_LIBRARY}
    ${SDL2_mixer_LIBRARY}
    ${OPENGL_LIBRARIES}
    ${assimp_LIBRARY}
)