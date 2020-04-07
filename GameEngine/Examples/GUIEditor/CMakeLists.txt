cmake_minimum_required(VERSION 3.17)

set(EDITOR_EXE_NAME "Editor")

#------------------------------------------------------------------------
# set source files
#------------------------------------------------------------------------
message("Adding source files")
set(EDITOR_SRC_LIST
    EditorDialog.cpp GUIEditor.cpp
    ) 

#------------------------------------------------------------------------
# create editor executable
#------------------------------------------------------------------------
if(WIN32)
    list(APPEND EDITOR_SRC_LIST windowsEditorMain.cpp)
    add_executable(${EDITOR_EXE_NAME} WIN32 ${EDITOR_SRC_LIST})
else()
    list(APPEND EDITOR_SRC_LIST linuxEditorMain.cpp)
    add_executable(${EDITOR_EXE_NAME} ${EDITOR_SRC_LIST})
endif(WIN32)

#------------------------------------------------------------------------
# set include paths
#------------------------------------------------------------------------
message("Setting ${ENGINE_NAME} include directories")
target_include_directories(${EDITOR_EXE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../")

#------------------------------------------------------------------------
# Set how to link Game engine
#------------------------------------------------------------------------
target_link_libraries(${EDITOR_EXE_NAME} ${ENGINE_NAME})

target_precompile_headers(${EDITOR_EXE_NAME} REUSE_FROM ${ENGINE_NAME})
