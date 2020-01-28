cmake_minimum_required(VERSION 3.15.0)
cmake_policy(SET CMP0091 NEW) #for unit testing

project(WeilanEngine)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILEoCOMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DRELEASE")

#Extern Includes and Libs
find_package(OpenGL REQUIRED)
find_library(assimp_LIBRARY assimp hint Extern/assimp/lib/windows)
find_library(SDL2_mixer_LIBRARY SDL2_mixer hint Extern/SDL2_mixer/lib)
find_library(SDL2_LIBRARY SDL2 hint Extern/SDL2/lib/x64 Extern/SDL2/lib/x86)

#Extern CMake Projects
add_subdirectory(Extern/glad/)
add_subdirectory(Extern/freetype/)
add_subdirectory(Extern/imgui/)
add_subdirectory(Extern/lua/)

#Engine
add_library(WeilanEngine)

add_subdirectory(Header/)
add_subdirectory(Source/)

get_target_property(ENGINE_SOURCES WeilanEngine SOURCES)

foreach(FILE ${ENGINE_SOURCES})
    source_group(
        TREE ${CMAKE_CURRENT_SOURCE_DIR}
        FILES ${FILE}
    )
endforeach()

target_include_directories(WeilanEngine
    PUBLIC 
    Extern/glm/
    Extern/json/include/
    Extern/dirent/include/
    Extern/SDL2/include/
    Extern/glad/include/
    Extern/assimp/include/
    Extern/freetype/include/
    Extern/stb/include/
    Extern/SDL2_mixer/include/
)

TARGET_LINK_LIBRARIES(WeilanEngine
    glad
    freetype
    imGUI
    lua
    ${SDL2_LIBRARY}
    ${SDL2_mixer_LIBRARY}
    ${OPENGL_LIBRARIES}
    ${assimp_LIBRARY}
)

add_compile_definitions(ROOT_DIR=std::string\(\"${CMAKE_CURRENT_SOURCE_DIR}\"\))

#Test
add_subdirectory(Test/)

