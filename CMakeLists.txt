cmake_minimum_required( VERSION 3.10 )

project( world )

find_package( OpenGL REQUIRED )

include_directories( ${OPENGL_INCLUDE_DIRS} )

set( GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" )

add_subdirectory( glfw )
add_subdirectory( glad/ )
add_subdirectory( glm )

if( MSVC )
    SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup" )
    SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:CONSOLE" )
endif()

set( SIMULATOR-SRC
     main.cpp
     utils.h
     )

if(WIN32)
    add_executable( world WIN32 ${SIMULATOR-SRC} )
else()
    add_executable( world MACOSX_BUNDLE ${SIMULATOR-SRC} )
endif()

target_link_libraries( world ${OPENGL_LIBRARIES} glfw )
target_link_libraries( world glad )
target_link_libraries( world glm )
if( MSVC )
    if(${CMAKE_VERSION} VERSION_LESS "3.6.0") 
        message( "\n\t[ WARNING ]\n\n\tCMake version lower than 3.6.\n\n\t - Please update CMake and rerun; OR\n\t - Manually set 'glfw-game' as StartUp Project in Visual Studio.\n" )
    else()
        set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT world )
    endif()
else()
    set(CMAKE_CXX_FLAGS "-std=c++0x")
    set(CMAKE_CXX_STANDARD 11)
endif()
