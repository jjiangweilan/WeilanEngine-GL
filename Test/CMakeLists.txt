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
)


#set_property(TARGET EngineTest PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_link_libraries(EngineTest
    gtest
    gmock
    KuangyeEngine
)

#Copy DLL

file(GLOB SDL2_mixer_lib
    ${CMAKE_CURRENT_SOURCE_DIR}/../Extern/SDL2_mixer/lib/*.dll
)

foreach(FILE ${SDL2_mixer_lib})
        configure_file(${FILE} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
endforeach()
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../Extern/assimp/lib/windows/assimp-vc140-mt.dll ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../Extern/SDL2/lib/x64/SDL2.dll ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

add_subdirectory(Graphics/)

