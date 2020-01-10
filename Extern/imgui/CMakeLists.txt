cmake_minimum_required(VERSION 3.15.0)
project(imGUI)

add_library(imGUI)

target_include_directories(imGUI
    INTERFACE
    Include/
    PRIVATE
    ../SDL2/include
    ../glad/include
    Include/imgui
)

target_link_libraries(imGUI 
    ${SDL2_LIBRARY}
)

add_subdirectory(Source/)
