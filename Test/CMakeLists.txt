cmake_minimum_required(VERSION 3.15.0)

add_subdirectory(gtest)
add_executable(EngineTest
    TestMain.cpp
)

target_include_directories(EngineTest
    PRIVATE
    gtest/googletest/include
    gtest/googlemock/include
    ../Header/Private
    Resource/Script
)

set_property(TARGET EngineTest PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set_property(TARGET EngineTest PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

#set_property(TARGET EngineTest PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_link_libraries(EngineTest
    gtest
    gmock
    WeilanEngine
)

#Copy DLL
set(DLL_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>")


file(GLOB SDL2_MIXER_DLL
    ${CMAKE_CURRENT_SOURCE_DIR}/../Extern/SDL2_mixer/lib/*.dll
)

add_custom_command(TARGET EngineTest POST_BUILD        
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  
    ${SDL2_MIXER_DLL}
    ${CMAKE_CURRENT_SOURCE_DIR}/../Extern/assimp/lib/windows/assimp-vc140-mt.dll 
    ${CMAKE_CURRENT_SOURCE_DIR}/../Extern/SDL2/lib/x64/SDL2.dll
    ${DLL_OUTPUT_DIR})                      

add_subdirectory(Graphics/)
add_subdirectory(GameTest/)
add_subdirectory(Resource/)
