cmake_minimum_required(VERSION 3.5.0)
set(CMAKE_C_COMPILER "/usr/bin/gcc")


project(SoundTest)


# set(SSL_PATH, D://Programming/Perl/Application/c/bin)

# set(CMAKE_C_COMPILER "gcc")


# option(CURL_USE_SCHANNEL "Use windows default SSL libraries" ON)

# option(CMAKE_USE_WINSSL "Set to ON to use WINSSL for windows." ON)

option(WITH_GUI "Build the project with GUI support" ON)
option(RASPI "Build the project with GPIO support" OFF)




add_subdirectory(portaudio)
add_subdirectory(iir1)
add_subdirectory(DaisySP)

set(SOURCES
    main.cpp
    nodes.cpp
    synth.cpp
    nodes_rotary_encoder.cpp
)
    
    if(WITH_GUI)
    message(STATUS "Building with GUI support")
    
    add_subdirectory(raylib)
    add_subdirectory(imgui)
    add_subdirectory(rlImGui)
    add_subdirectory(imgui-node-editor)
    
    set(SOURCES ${SOURCES}
        imguiAddons.cpp

        nodes_draw.cpp
        gui.cpp
    )


else()
    message(STATUS "Building in headless mode (no GUI)")
endif()

if(RASPI) 
    message(STATUS "Building with GPIO support")
    add_subdirectory(pigpio)

    set(SOURCES ${SOURCES}
        # rotary_encoder.cpp
    )

else ()
    message(STATUS "Building without GPIO support")
endif()


# add_subdirectory(curl-8.10.1)

#main.cpp imguiAddons.cpp nodes.cpp synth.cpp nodes_draw.cpp gui.cpp
add_executable(SoundTest ${SOURCES})





target_include_directories(SoundTest PUBLIC ${CMAKE_SOURCE_DIR}/iir1/)
target_include_directories(SoundTest PUBLIC ${CMAKE_SOURCE_DIR}/DaisySP/)
target_include_directories(SoundTest PUBLIC ${CMAKE_SOURCE_DIR}/portaudio/include/)



if(WITH_GUI)

# Add headless libraries or flags
    target_compile_definitions(SoundTest PUBLIC "WITH_GUI")

    target_include_directories(SoundTest PUBLIC ${CMAKE_SOURCE_DIR}/imgui/)
    target_include_directories(SoundTest PUBLIC ${CMAKE_SOURCE_DIR}/rlImGui/)
    target_include_directories(SoundTest PUBLIC ${CMAKE_SOURCE_DIR}/imgui-node-editor/)
    target_include_directories(SoundTest PUBLIC ${CMAKE_SOURCE_DIR}/raylib/)

endif()


if(RASPI) 
    target_compile_definitions(SoundTest PUBLIC "RASPI")


    target_include_directories(SoundTest PUBLIC ${CMAKE_SOURCE_DIR}/pigpio/)

endif()



target_link_libraries(SoundTest LINK_PUBLIC iir::iir_static)
target_link_libraries(SoundTest LINK_PUBLIC DaisySP)
target_link_libraries(SoundTest LINK_PUBLIC portaudio)


if(WITH_GUI)

    target_link_libraries(SoundTest LINK_PUBLIC NodeEditor)
    target_link_libraries(SoundTest LINK_PUBLIC raylib)  
    target_link_libraries(SoundTest LINK_PUBLIC imgui)
    target_link_libraries(SoundTest LINK_PUBLIC imguiimpl)

endif()

if(RASPI)

    target_link_libraries(SoundTest LINK_PUBLIC pigpio)

endif()



