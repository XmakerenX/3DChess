cmake_minimum_required(VERSION 3.17)

set(TEST_EXE_NAME "Test")

#------------------------------------------------------------------------
# set source files
#------------------------------------------------------------------------
message("Adding source files")
set(TEST_SRC_LIST
    TestingGame.cpp
    ) 

#------------------------------------------------------------------------
# create editor executable
#------------------------------------------------------------------------
if(WIN32)
    list(APPEND TEST_SRC_LIST windowsTestMain.cpp)
    add_executable(${TEST_EXE_NAME} WIN32 ${TEST_SRC_LIST})
else()
    list(APPEND TEST_SRC_LIST linuxTestMain.cpp)
    add_executable(${TEST_EXE_NAME} ${TEST_SRC_LIST})
endif(WIN32)

#------------------------------------------------------------------------
# set include paths
#------------------------------------------------------------------------
target_include_directories(${TEST_EXE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../")

target_precompile_headers(${TEST_EXE_NAME} REUSE_FROM ${ENGINE_NAME})

#------------------------------------------------------------------------
# Set how to link Game engine
#------------------------------------------------------------------------
target_link_libraries(${TEST_EXE_NAME} ${ENGINE_NAME})
