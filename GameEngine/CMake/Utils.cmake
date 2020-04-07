cmake_minimum_required(VERSION 3.2)

#------------------------------------------------------------------------
# get_cxx_flags - returns in ret the CXX flags for the current build type
#------------------------------------------------------------------------
function(get_cxx_flags ret)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(${ret} ${CMAKE_CXX_FLAGS_DEBUG} PARENT_SCOPE)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        set(${ret} ${CMAKE_CXX_FLAGS_RELEASE} PARENT_SCOPE)
    elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo") 
        set(${ret} ${CMAKE_CXX_FLAGS_RELWITHDEBINFO} PARENT_SCOPE)
    elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
        set(${ret} ${CMAKE_CXX_FLAGS_MINSIZEREL} PARENT_SCOPE)
    else()
    set(${ret} ${CMAKE_CXX_FLAGS_DEBUG} PARENT_SCOPE)
    endif(CMAKE_BUILD_TYPE STREQUAL "Debug")
endfunction(get_cxx_flags)

#------------------------------------------------------------------------
# createPreCompiledheader - create the precompiled header pch/gch file
#------------------------------------------------------------------------
macro(createPreCompiledheaders target headerName sourcePath pchPath)
    if (MSVC)
        set_source_files_properties(${sourcePath} PROPERTIES
        COMPILE_FLAGS "/Fp${pchPath} /Yc${headerName}"
        OBJECT_OUTPUTS "${pchPath}")        
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        # copy header file to build folder
        add_custom_command(OUTPUT ${headerName}
                           COMMAND ${CMAKE_COMMAND} -E copy
                           ${CMAKE_CURRENT_SOURCE_DIR}/${headerName} ${CMAKE_CURRENT_BINARY_DIR}/${headerName}
                           DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${headerName}"
                           COMMENT "Copying ${headerName} to ${CMAKE_CURRENT_BINARY_DIR}")
        
        # get target include flags
        get_target_property(includeDirs ${target} INCLUDE_DIRECTORIES)
        set(includeFlags "")
        foreach(includeDir ${includeDirs})
            list(APPEND includeFlags "-I${includeDir}")
        endforeach(includeDir)
        
        # get complie flags
        get_cxx_flags(complie_flags)
        string(REGEX REPLACE " " ";" complie_flags ${complie_flags})
        
        # create precomplied header command
        set(complie_cmd COMMAND "${CMAKE_CXX_COMPILER}" )
        list(APPEND complie_cmd "${complie_flags}" ${includeFlags})
        list(APPEND complie_cmd -x c++-header ${CMAKE_CURRENT_BINARY_DIR}/${headerName} -o ${pchPath})
                        
        add_custom_command(
        OUTPUT "${pchPath}"
        ${complie_cmd}
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${headerName}"
        COMMENT "Precompiling pre compiled header")
    endif(MSVC)
endmacro(createPreCompiledheaders)

#------------------------------------------------------------------------
# setPrecompliedHeader - set sourceList to use the given precompiled
#						 header and force include it
#------------------------------------------------------------------------
macro(setPrecompliedHeaders headerPath pchPath sourceList)
    foreach(_source ${sourceList})
        if(MSVC)
            set_source_files_properties(${_source} PROPERTIES
                COMPILE_FLAGS "/Fp${pchPath} /Yu${headerPath} /FI${headerPath}"
                OBJECT_DEPENDS "${pchPath}")
        elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
            set_source_files_properties(${_source} PROPERTIES
                COMPILE_FLAGS "-include ${CMAKE_CURRENT_BINARY_DIR}/${headerPath}"
                OBJECT_DEPENDS "${pchPath}")
        endif(MSVC)
    endforeach()
endmacro(setPrecompliedHeaders)

#------------------------------------------------------------------------
# generateWaylandFiles - generate c files for wayland unstable protocols
#------------------------------------------------------------------------
macro(generateWaylandFiles WAYLAND_PROTOCOLS)
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
    
    set(SRC_WAYLAND_LIST "${CMAKE_CURRENT_BINARY_DIR}/WaylandGen/xdg-shell-unstable-v6-protocol.c")
    #include_directories("${CMAKE_CURRENT_BINARY_DIR}/WaylandGen")
    add_library(${WAYLAND_PROTOCOLS} ${SRC_WAYLAND_LIST})
    target_include_directories(${WAYLAND_PROTOCOLS} SYSTEM PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/WaylandGen)
endmacro(generateWaylandFiles) 
