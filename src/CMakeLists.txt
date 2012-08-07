cmake_minimum_required(VERSION 2.6)
set(PROJECT_NAME "Glassomium")

project(${PROJECT_NAME})

MACRO(ADD_MSVC_PRECOMPILED_HEADER PrecompiledHeader PrecompiledSource SourcesVar)
  IF(MSVC)
    GET_FILENAME_COMPONENT(PrecompiledBasename ${PrecompiledHeader} NAME_WE)
    SET(PrecompiledBinary "$(IntDir)/${PrecompiledBasename}.pch")
    SET(Sources ${${SourcesVar}})

    SET_SOURCE_FILES_PROPERTIES(${PrecompiledSource}
                                PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_OUTPUTS "${PrecompiledBinary}")
    SET_SOURCE_FILES_PROPERTIES(${Sources}
                                PROPERTIES COMPILE_FLAGS "/Yu\"${PrecompiledHeader}\" /FI\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_DEPENDS "${PrecompiledBinary}")  
    # Add precompiled header to SourcesVar
    LIST(APPEND ${SourcesVar} ${PrecompiledSource})
  ENDIF(MSVC)
ENDMACRO(ADD_MSVC_PRECOMPILED_HEADER)


#64 or 32?
set(BITNESS 32)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(BITNESS 64)
endif()

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/;${CMAKE_MODULE_PATH}")

if (CMAKE_BUILD_TYPE STREQUAL "")
  # CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
  # differentiation between debug and release builds.
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

set(BUILD_SHARED_LIBS FALSE)

# Glassomium UI

set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
if (WIN32)
	set(SFMLDIR "${CMAKE_SOURCE_DIR}/lib/sfml-win32")
endif()
if (UNIX)
	set(SFMLDIR "${CMAKE_SOURCE_DIR}/lib/sfml-linux${BITNESS}")
endif()
find_package(SFML COMPONENTS system network window graphics REQUIRED)
 
if(NOT SFML_FOUND)
	message(SEND_ERROR "Failed to find SFML.")
endif() 

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/dist")
 

set(HDRS
	Application.h
    Appinfo.h
    Globals.h
    Rectangle.h
    Utils.h
    WebView.h
    Window.h
    UIManager.h
    TuioManager.h
    PointerSprite.h
    ConcurrentQueue.h
    TouchEvent.h
    GestureManager.h
    TouchGroup.h
    Gesture.h
    DragGesture.h
    GestureEvent.h
    TwoFingerGesture.h
    FileManager.h
    TouchGesture.h
    SystemWindow.h
    AnimationManager.h
    Animation.h
    ColorChangeAnimation.h
    Configuration.h
    KeyboardWindow.h
    BrowserWindow.h
    TransformAnimation.h
    ScreensaverWindow.h
    AppConfiguration.h
    ThemeConfiguration.h
    ServerManager.h
    UserWindow.h
    Launcher.h
    Blob.h
    stdafx.h
)
 
set(SRCS
	Application.cpp
    Utils.cpp
    WebView.cpp
    Window.cpp
    UIManager.cpp
    TuioManager.cpp
    PointerSprite.cpp
    GestureManager.cpp
    TouchGroup.cpp
    Gesture.cpp
    DragGesture.cpp
    TwoFingerGesture.cpp
    FileManager.cpp
    TouchGesture.cpp
    SystemWindow.cpp
    AnimationManager.cpp
    Animation.cpp
    ColorChangeAnimation.cpp
    Configuration.cpp
    KeyboardWindow.cpp
    BrowserWindow.cpp
    TransformAnimation.cpp
    ScreensaverWindow.cpp
    AppConfiguration.cpp
    ThemeConfiguration.cpp
    ServerManager.cpp
    UserWindow.cpp
    Launcher.cpp
    Blob.cpp
)

include_directories(./include)
include_directories(./include/sfgui/include)

# Add precompiled headers on windows and icon resource
if (WIN32)
	ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" SRCS)

  SET(SRCS ${SRCS} resources.rc)
endif()

add_executable(${PROJECT_NAME} ${HDRS} ${SRCS})

set(PROJ_COMPILE_FLAGS "-DSFML_STATIC")

# Precompiled headers on linux
if (UNIX)

# Warn about invalid precompiled headers
add_definitions(${CMAKE_CXX_FLAGS} "-Winvalid-pch -Wno-shadow -Wno-unused-parameter") #-Werror -H

add_custom_command(TARGET ${PROJECT_NAME}
	PRE_BUILD
    
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/create_precompiled_header.rb "${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h.gch" "\"${CMAKE_CXX_COMPILER} ${CMAKE_CXX_FLAGS} -DSFML_STATIC -I${CMAKE_CURRENT_SOURCE_DIR}/include -c -O2 -g -x c++-header ${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h -o ${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h.gch\""
)

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "${PROJ_COMPILE_FLAGS} -include ${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h")
endif()

if (WIN32)
  target_link_libraries(${PROJECT_NAME} Opengl32.lib) 
  target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/lib/berkelium-win32/lib/berkelium.lib)
endif()
if (UNIX)
  set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-lGLU -Wl,-rpath=./")
  target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/lib/berkelium-linux${BITNESS}/lib/liblibberkelium.so)
endif()

include_directories(${SFML_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} TUIO osc ip ${SFML_LIBRARIES} sfgui)

add_subdirectory(include/TUIO)
add_subdirectory(include/osc)
add_subdirectory(include/ip)

set(SFGUI_STATIC TRUE)
set(SFGUI_STATIC_LIBRARIES TRUE)
set(BUILD_SHARED_LIBS FALSE)
add_subdirectory(include/sfgui)

 
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dist/bin)
 
# post-build copy for win32
if(WIN32)
    add_custom_command( TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND if not exist .\\dist\\bin mkdir .\\dist\\bin )
    add_custom_command( TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND copy \"$(OutputPath)\\*\" .\\dist\\bin )

endif()

if(UNIX)
    set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/dist/bin)
endif(UNIX)
 
if(WIN32)
    install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
    )

	# Copy berkelium stuff
	
	FILE(GLOB bk_dll "${CMAKE_SOURCE_DIR}/lib/berkelium-win32/bin/*.dll")
	install(FILES ${bk_dll} 
		DESTINATION bin
	
	)
	
	FILE(GLOB bk_exe "${CMAKE_SOURCE_DIR}/lib/berkelium-win32/bin/*.exe")
	install(FILES ${bk_exe} 
		DESTINATION bin
	
	)

	FILE(GLOB bk_pak "${CMAKE_SOURCE_DIR}/lib/berkelium-win32/bin/*.pak")
	install(FILES ${bk_pak} 
		DESTINATION bin
	
	)

	install(DIRECTORY ${CMAKE_SOURCE_DIR}/lib/berkelium-win32/bin/locales
		DESTINATION bin
	
	)
endif(WIN32)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/dist
    DESTINATION ../
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/server
    DESTINATION ./
)

if(UNIX)
 
	install(FILES ${CMAKE_SOURCE_DIR}/lib/berkelium-linux${BITNESS}/bin/berkelium
		${CMAKE_SOURCE_DIR}/lib/berkelium-linux${BITNESS}/bin/chrome.pak
		${CMAKE_SOURCE_DIR}/lib/berkelium-linux${BITNESS}/bin/resources.pak
		${CMAKE_SOURCE_DIR}/lib/berkelium-linux${BITNESS}/bin/libffmpegsumo.so
		${CMAKE_SOURCE_DIR}/lib/berkelium-linux${BITNESS}/lib/liblibberkelium.so
		DESTINATION bin
	)

	
	FILE(GLOB sfml_libs "${CMAKE_SOURCE_DIR}/lib/sfml-linux${BITNESS}/lib/*.so.2")
	install(FILES ${sfml_libs} 
		DESTINATION bin
	)
 
	set(CHMOD_COMMAND_BK "chmod +x ${PROJECT_BINARY_DIR}/dist/bin/berkelium")
 	install(CODE "execute_process( COMMAND ${CHMOD_COMMAND_BK} )")

    set(CHMOD_COMMAND_LA "chmod +x ${PROJECT_BINARY_DIR}/dist/Launcher.sh")
    install(CODE "execute_process( COMMAND ${CHMOD_COMMAND_LA} )")
	  
endif(UNIX)
 
