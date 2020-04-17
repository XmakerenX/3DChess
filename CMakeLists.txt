project(3DChess)
cmake_minimum_required(VERSION 3.17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CHESS_ENGINE_NAME "ChessEngine")

find_package(Boost QUIET REQUIRED)

#------------------------------------------------------------------------
# set source files
#------------------------------------------------------------------------
set(CHESS_ENGINE_SRC_LIST
    Chess/ChessEngine/Bishop.cpp
    Chess/ChessEngine/Board.cpp
    Chess/ChessEngine/King.cpp
    Chess/ChessEngine/Knight.cpp
    Chess/ChessEngine/Pawn.cpp
    Chess/ChessEngine/Piece.cpp
    Chess/ChessEngine/Queen.cpp
    Chess/ChessEngine/Rook.cpp
    ) 

set(CHESS_SRC_LIST
    Chess/Chess.cpp
    Chess/ChessScene.cpp
    Chess/OptionDialog.cpp
    ) 
    
if (WIN32)
    set(CHESS_SRC_LIST ${CHESS_SRC_LIST} Chess/rc.rc)
    set_source_files_properties(pawn.iso Chess/rc.rc2 PROPERTIES LANGUAGE RC)
    list(APPEND CHESS_SRC_LIST Chess/windowsGameMain.cpp)
    #aux_source_directory(./Chess/Windows CHESS_SRC_LIST)
else()
    list(APPEND CHESS_SRC_LIST Chess/linuxGameMain.cpp)
    #aux_source_directory(./Chess/Linux CHESS_SRC_LIST)
endif(WIN32)

#------------------------------------------------------------------------
# set libraries
#------------------------------------------------------------------------
add_library(${CHESS_ENGINE_NAME} ${CHESS_ENGINE_SRC_LIST})
target_include_directories(${CHESS_ENGINE_NAME} SYSTEM PUBLIC ${Boost_INCLUDE_DIRS})

add_subdirectory(GameEngine)

#------------------------------------------------------------------------
# set executables
#------------------------------------------------------------------------
if (WIN32)
    add_executable(${PROJECT_NAME} WIN32 ${CHESS_SRC_LIST})

else()
    add_executable(${PROJECT_NAME} ${CHESS_SRC_LIST})
endif(WIN32)

target_include_directories(${PROJECT_NAME} PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/GameEngine")

target_precompile_headers(${PROJECT_NAME} REUSE_FROM GameEngine)

#------------------------------------------------------------------------
# Set how to link Game engine
#------------------------------------------------------------------------

target_link_libraries(${PROJECT_NAME} ${CHESS_ENGINE_NAME})
target_link_libraries(${PROJECT_NAME} GameEngine)
