project(GlxOpenGL)
cmake_minimum_required(VERSION 3.2)

function(was_found package_name isfound) 
    #message("was_found was called " ${package_name} " " ${isfound})
    if(NOT isfound)
        message("The package " ${package_name} " was not found")
    endif(NOT isfound)
endfunction(was_found)

macro(generateWaylandFiles)
    if(WAYLAND_FOUND)
        execute_process(
            COMMAND ${PKG_CONFIG_EXECUTABLE} --variable=wayland_scanner wayland-scanner
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            RESULT_VARIABLE WAYLAND_SCANNER_RC
            OUTPUT_VARIABLE WAYLAND_SCANNER
            ERROR_QUIET
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        if(NOT WAYLAND_SCANNER_RC EQUAL 0)
            set(WAYLAND_FOUND FALSE)
        endif()
    endif()
    
    if(WAYLAND_FOUND)
      execute_process(
        COMMAND ${PKG_CONFIG_EXECUTABLE} --variable=pkgdatadir wayland-protocols
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        RESULT_VARIABLE WAYLAND_PROTOCOLS_DIR_RC
        OUTPUT_VARIABLE WAYLAND_PROTOCOLS_DIR
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
      )
      if(NOT WAYLAND_PROTOCOLS_DIR_RC EQUAL 0)
        set(WAYLAND_FOUND FALSE)
      endif()
    endif()
    
    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/WaylandGen/xdg-shell-unstable-v6-protocol.h"
        DEPENDS "${WAYLAND_PROTOCOLS_DIR}/unstable/xdg-shell/xdg-shell-unstable-v6.xml"
        COMMAND "${WAYLAND_SCANNER}"
        ARGS client-header "${WAYLAND_PROTOCOLS_DIR}/unstable/xdg-shell/xdg-shell-unstable-v6.xml" "${CMAKE_CURRENT_BINARY_DIR}/WaylandGen/xdg-shell-unstable-v6-protocol.h"
    )

    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/WaylandGen/xdg-shell-unstable-v6-protocol.c"
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/WaylandGen/xdg-shell-unstable-v6-protocol.h"
        COMMAND "${WAYLAND_SCANNER}"
        ARGS private-code "${WAYLAND_PROTOCOLS_DIR}/unstable/xdg-shell/xdg-shell-unstable-v6.xml" "${CMAKE_CURRENT_BINARY_DIR}/WaylandGen/xdg-shell-unstable-v6-protocol.c"
        
    )
    
    set(SRC_LIST ${SRC_LIST} "${CMAKE_CURRENT_BINARY_DIR}/WaylandGen/xdg-shell-unstable-v6-protocol.c")
    include_directories("${CMAKE_CURRENT_BINARY_DIR}/WaylandGen")
endmacro(generateWaylandFiles)

# make cmake find our *.cmake files
LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake/)

set(CHESS_ENGINE_NAME "ChessEngine")
set(ENGINE_NAME "Engine")
set(EDITOR_EXE_NAME "Editor")
set(TEST_EXE_NAME "Test")

message("Loading libraries")
if (UNIX)
    find_package(X11 QUIET REQUIRED)
    find_package(Xrandr QUIET REQUIRED)
    find_package(FontConfig QUIET REQUIRED)
    find_package(Wayland QUIET REQUIRED)
    find_package(XKBCommon QUIET REQUIRED)
    find_package(EGL QUIET REQUIRED)
    set(OpenGL_GL_PREFERENCE GLVND)
    find_package(PkgConfig QUIET REQUIRED)
    was_found("PkgConfig" ${PKG_CONFIG_FOUND})
    generateWaylandFiles()
endif(UNIX)
find_package(Threads QUIET REQUIRED)
find_package(OpenGL QUIET REQUIRED)
find_package(Freetype QUIET REQUIRED)
find_package(Boost QUIET REQUIRED)
find_package(GLEW QUIET REQUIRED)
find_package(PNG QUIET REQUIRED)
find_package(JPEG QUIET REQUIRED)
find_package(FBX QUIET REQUIRED)
# Print error if one of the packges was not found
was_found("Threads" ${Threads_FOUND})
was_found("OpenGL" ${OPENGL_FOUND})
was_found("FreeType" ${FREETYPE_FOUND})
was_found("Boost" ${Boost_FOUND})
was_found("GLEW" ${GLEW_FOUND})
was_found("PNG" ${PNG_FOUND})
was_found("JPEG" ${JPEG_FOUND})
was_found("FBXSDK" ${FBXSDK_FOUND})
if (UNIX)
    was_found("X11" ${X11_FOUND})
    was_found("Xrandr" ${XRANDR_FOUND})
    was_found("FontConfig" ${FONTCONFIG_FOUND})
    was_found("Wayland" ${WAYLAND_FOUND})
    was_found("XKBCommon" ${XKBCOMMON_FOUND})
    was_found("EGL" ${EGL_FOUND})
endif(UNIX)

message("Setting include directories")
include_directories(${FREETYPE_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_DIR})
include_directories(${PNG_INCLUDE_DIRS})
include_directories(${JPEG_INCLUDE_DIRS})
include_directories(${FBXSDK_INCLUDE_DIR})
if (UNIX)
    include_directories(${X11_INCLUDE_DIR})
    include_directories(${XRANDR_INCLUDE_DIRS})
    include_directories(${FONTCONFIG_INCLUDE_DIRS})
    include_directories(${WAYLAND_CLIENT_INCLUDE_DIR})
    include_directories(${WAYLAND_EGL_INCLUDE_DIR})
    include_directories(${XKBCOMMON_INCLUDE_DIRS})
    include_directories(${EGL_INCLUDE_DIR})
endif(UNIX)

message("adiing source files")
aux_source_directory(. SRC_LIST)
aux_source_directory(./GUI SRC_LIST)
aux_source_directory(./ChessEngine/ CHESS_SRC_LIST)
aux_source_directory(./Main MAIN_SRC_LIST)
aux_source_directory(./Editor EDITOR_SRC_LIST)
aux_source_directory(./Test/ TEST_SRC_LIST)
if (WIN32)
    aux_source_directory(./Windows SRC_LIST)
    aux_source_directory(./Main/Windows MAIN_SRC_LIST)
    aux_source_directory(./Editor/Windows EDITOR_SRC_LIST)
    aux_source_directory(./Test/Windows TEST_SRC_LIST)
else()
    aux_source_directory(./Linux SRC_LIST)
    aux_source_directory(./Main/Linux MAIN_SRC_LIST)
    aux_source_directory(./Build/Linux MAIN_SRC_LIST)
    aux_source_directory(./Editor/Linux EDITOR_SRC_LIST)
    aux_source_directory(./Test/Linux TEST_SRC_LIST)
endif(WIN32)

message("setting chess library")
add_library(${CHESS_ENGINE_NAME} ${CHESS_SRC_LIST})

message("setting engine library")
add_library(${ENGINE_NAME} ${SRC_LIST})
target_link_libraries(${ENGINE_NAME} ${OPENGL_LIBRARIES})
target_link_libraries(${ENGINE_NAME} ${FREETYPE_LIBRARIES})
target_link_libraries(${ENGINE_NAME} ${GLEW_LIBRARIES})
target_link_libraries(${ENGINE_NAME} ${PNG_LIBRARIES})
target_link_libraries(${ENGINE_NAME} ${JPEG_LIBRARIES})
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(${ENGINE_NAME} ${FBXSDK_LIBRARY_DEBUG})
else()
    target_link_libraries(${ENGINE_NAME} ${FBXSDK_LIBRARY})
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")
if (UNIX)
    target_link_libraries(${ENGINE_NAME} ${X11_LIBRARIES})
    target_link_libraries(${ENGINE_NAME} ${XRANDR_LIBRARIES})
    target_link_libraries(${ENGINE_NAME} ${FONTCONFIG_LIBRARIES})
    target_link_libraries(${ENGINE_NAME} ${WAYLAND_LIBRARIES})
    target_link_libraries(${ENGINE_NAME} ${WAYLAND_EGL_LIBRARIES})
    target_link_libraries(${ENGINE_NAME} ${XKBCOMMON_LIBRARIES})
    target_link_libraries(${ENGINE_NAME} ${EGL_LIBRARY})
endif(UNIX)
target_link_libraries(${ENGINE_NAME} ${CMAKE_THREAD_LIBS_INIT})

message("setting executable")
if (WIN32)
    add_executable(${PROJECT_NAME} WIN32 ${MAIN_SRC_LIST})
    add_executable(${EDITOR_EXE_NAME} WIN32 ${EDITOR_SRC_LIST})
    add_executable(${TEST_EXE_NAME} WIN32 ${TEST_SRC_LIST})
else()
    add_executable(${PROJECT_NAME} ${MAIN_SRC_LIST})
    add_executable(${EDITOR_EXE_NAME} ${EDITOR_SRC_LIST})
    add_executable(${TEST_EXE_NAME} ${TEST_SRC_LIST})
endif(WIN32)

target_link_libraries(${PROJECT_NAME} ${CHESS_ENGINE_NAME})
target_link_libraries(${PROJECT_NAME} ${ENGINE_NAME})
target_link_libraries(${EDITOR_EXE_NAME} ${ENGINE_NAME}) 
target_link_libraries(${TEST_EXE_NAME} ${ENGINE_NAME})
