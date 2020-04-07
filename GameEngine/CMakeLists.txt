cmake_minimum_required(VERSION 3.17)

project(GameEngine)
# make cmake find the project *.cmake files
LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake/)
# ignore warning about <PackageName> does not match CMAKE_FIND_PACKAGE_NAME
set(FPHSA_NAME_MISMATCHED 1)


include(Utils)

set(ENGINE_NAME "GameEngine")
set(WAYLAND_PROTOCOLS_NAME "WaylandProtocols")
set(PrecompiledHeaderHeader "pch.h")

#------------------------------------------------------------------------
# Find all the needed packages
#------------------------------------------------------------------------
if (UNIX)
    find_package(X11 QUIET REQUIRED)
    find_package(Xrandr QUIET REQUIRED)
    find_package(FontConfig QUIET REQUIRED)
    find_package(Wayland QUIET REQUIRED)
    find_package(XKBCommon QUIET REQUIRED)
    find_package(EGL QUIET REQUIRED)
    find_package(LibXml2 QUIET REQUIRED)
    set(OpenGL_GL_PREFERENCE GLVND)
    find_package(PkgConfig QUIET REQUIRED)
    generateWaylandFiles(${WAYLAND_PROTOCOLS_NAME})
endif(UNIX)
find_package(Threads QUIET REQUIRED)
find_package(OpenGL QUIET REQUIRED)
find_package(Freetype QUIET REQUIRED)
find_package(Boost QUIET REQUIRED)
find_package(GLEW QUIET REQUIRED)
find_package(PNG QUIET REQUIRED)
find_package(JPEG QUIET REQUIRED)
message(STATUS "FBX = ${FBX}")
if (FBX)
    message(STATUS "FBX = ${FBX}")
    find_package(FBX QUIET REQUIRED)
endif(FBX)

#------------------------------------------------------------------------
# set source files
#------------------------------------------------------------------------
set(GAME_ENGINE_SRC
    timer.cpp 
    BaseGame.cpp
    AssetLoading/AssetManager.cpp
    AssetLoading/MeshGenerator.cpp
    AssetLoading/ObjLoader.cpp
    GameWindow/BaseWindow.cpp
    Render/Font.cpp
    Render/Mesh.cpp
    Render/Object.cpp
    Render/Scene.cpp
    Render/Shader.cpp
    Render/Shader.cpp 
    Render/Sprite.cpp
    Render/subMesh.cpp
    Render/Camera/Camera.cpp
    Render/Camera/FreeCam.cpp
    Render/GUI/ButtonUI.cpp
    Render/GUI/CheckboxUI.cpp
    Render/GUI/ComboBoxUI.cpp
    Render/GUI/ControlUI.cpp 
    Render/GUI/DialogUI.cpp
    Render/GUI/EditBoxUI.cpp
    Render/GUI/ListBoxUI.cpp
    Render/GUI/RadioButtonUI.cpp
    Render/GUI/ScrollBarUI.cpp
    Render/GUI/SliderUI.cpp
    Render/GUI/StaticUI.cpp
    ) 
    
if(WIN32)
    list(APPEND GAME_ENGINE_SRC GameWindow/Windows/WindowsGameWin.cpp)
else()
    list(APPEND GAME_ENGINE_SRC GameWindow/Linux/LinuxX11Window.cpp GameWindow/Linux/LinuxWaylandWindow.cpp)
endif(WIN32)

if(FBX)
    list(APPEND GAME_ENGINE_SRC AssetLoading/FbxLoader.cpp)
endif(FBX)

#------------------------------------------------------------------------
# create game engine library
#------------------------------------------------------------------------
add_library(${ENGINE_NAME} ${GAME_ENGINE_SRC})

#------------------------------------------------------------------------
# set include paths
#------------------------------------------------------------------------
target_include_directories(${ENGINE_NAME} SYSTEM PUBLIC ${FREETYPE_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${GLEW_INCLUDE_DIR} ${PNG_INCLUDE_DIRS} ${JPEG_INCLUDE_DIRS})
if (FBX)
	target_include_directories(${ENGINE_NAME} SYSTEM PUBLIC ${FBXSDK_INCLUDE_DIR})
endif(FBX)
if (UNIX)
    target_include_directories(${ENGINE_NAME} SYSTEM PUBLIC ${X11_INCLUDE_DIR} ${XRANDR_INCLUDE_DIRS} ${FONTCONFIG_INCLUDE_DIRS} ${WAYLAND_CLIENT_INCLUDE_DIR}
        ${WAYLAND_EGL_INCLUDE_DIR} ${XKBCOMMON_INCLUDE_DIRS} ${EGL_INCLUDE_DIR} ${LIBXML2_INCLUDE_DIRS})
endif(UNIX)

#------------------------------------------------------------------------
# Setting game engine complier definitionss 
#------------------------------------------------------------------------
if (FBX)
    target_compile_definitions(${ENGINE_NAME} PUBLIC FBX)
endif(FBX)
if (WIN32)
    target_compile_definitions(${ENGINE_NAME} PUBLIC _WIN32_WINNT=0x0A00
                                              PUBLIC  WINVER=0x0A00)
endif(WIN32)

#------------------------------------------------------------------------
# Setup precomplied header
#------------------------------------------------------------------------
target_precompile_headers(${ENGINE_NAME} PRIVATE ${PrecompiledHeaderHeader})

target_link_libraries(${ENGINE_NAME} ${OPENGL_LIBRARIES} ${FREETYPE_LIBRARIES} GLEW::glew ${PNG_LIBRARIES} ${JPEG_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

#------------------------------------------------------------------------
# Set how to link Game engine
#------------------------------------------------------------------------
if (FBX)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_libraries(${ENGINE_NAME} ${FBXSDK_LIBRARY_DEBUG})
    else()
        target_link_libraries(${ENGINE_NAME} ${FBXSDK_LIBRARY})
    endif(CMAKE_BUILD_TYPE STREQUAL "Debug")
endif(FBX)

if (UNIX)
    target_link_libraries(${ENGINE_NAME} ${X11_LIBRARIES} ${XRANDR_LIBRARIES} ${FONTCONFIG_LIBRARIES} ${WAYLAND_LIBRARIES}
                                         ${WAYLAND_EGL_LIBRARIES} ${XKBCOMMON_LIBRARIES} ${EGL_LIBRARY} ${LIBXML2_LIBRARIES} ${WAYLAND_PROTOCOLS_NAME})
endif(UNIX)

add_subdirectory(Examples)

#------------------------------------------------------------------------
# Copy FBX dll to library folder
#------------------------------------------------------------------------
if(WIN32 AND FBX)
    if (CMAKE_BUILD_TYPE MATCHES Debug)
        string(REPLACE "libfbxsdk-md.lib" "libfbxsdk.dll" fbx_dll_path ${FBXSDK_LIBRARY_DEBUG})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E copy
                           ${fbx_dll_path} $<TARGET_FILE_DIR:${PROJECT_NAME}>)
    else()
        string(REPLACE "libfbxsdk-md.lib" "libfbxsdk.dll" fbx_dll_path ${FBXSDK_LIBRARY})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E copy
                           ${fbx_dll_path} $<TARGET_FILE_DIR:${PROJECT_NAME}>)
    endif(CMAKE_BUILD_TYPE MATCHES Debug)
endif(WIN32 AND FBX)
