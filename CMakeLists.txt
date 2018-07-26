 ###############################################################################
 # Copyright (C) 2018 Charly Lamothe                                           #
 #                                                                             #
 # This file is part of LibSharedMemoryObject.                                 #
 #                                                                             #
 #   Licensed under the Apache License, Version 2.0 (the "License");           #
 #   you may not use this file except in compliance with the License.          #
 #   You may obtain a copy of the License at                                   #
 #                                                                             #
 #   http://www.apache.org/licenses/LICENSE-2.0                                #
 #                                                                             #
 #   Unless required by applicable law or agreed to in writing, software       #
 #   distributed under the License is distributed on an "AS IS" BASIS,         #
 #   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  #
 #   See the License for the specific language governing permissions and       #
 #   limitations under the License.                                            #
 ###############################################################################

# --- PROJECT ---

PROJECT(smo C)
cmake_minimum_required(VERSION 3.8)


# --- GLOBAL OPTIONS ---

set(CMAKE_VERBOSE_MAKEFILE on)
SET(ROOT_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE STRING "Default root path of the current build directory")


# --- DEPENDENCIES OPTIONS ---

SET(LIBEI_SYSTEM TRUE CACHE BOOL "Default LIBEI installation type")
set(LIBEI_INSTALL ${ROOT_BUILD_DIR}/libei/install CACHE STRING "Default LIBEI install path")

SET(LIBUEUM_SYSTEM TRUE CACHE BOOL "Default LIBUEUM installation type")
set(LIBUEUM_INSTALL ${ROOT_BUILD_DIR}/libueum/install CACHE STRING "Default LIBUEUM install path")


# --- PATH ---

set(${CMAKE_CURRENT_SOURCE_DIR} ..)

set(SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")

set(EXAMPLES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/examples")

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin/release")
    set(EXAMPLES_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin/release")
elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin/debug")
    set(EXAMPLES_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin/debug")
endif ()


# --- COMPILE OPTIONS ---

set(CMAKE_C_FLAGS "-fPIC")

# Compile options for Microsoft Visual Studio
if (MSVC)
    # Add O2 optimization if build type is Release
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options("$<$<CONFIG:RELEASE>:/O2>")
    endif ()
    add_compile_options("/W4")
    if (MSVC_VERSION GREATER_EQUAL 1900)
        add_compile_options("/permissive-") # Add /permissive- (kind of -pedantic) available since Visual Studio 2017
    endif ()
    add_compile_options("/W4") 
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON) # This variable is used to initialize the property on each target as it is created
    set(CMAKE_C_FLAGS "${CMAKE_ENABLE_C11}") # Compile with C11 rules

    # Use secure functions by defaualt and suppress warnings about
    # deprecated" functions
    # TODO: try to remove this flags and fix the issue raised
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /D _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /D _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT=1")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /D _CRT_NONSTDC_NO_WARNINGS=1 /D _CRT_SECURE_NO_WARNINGS=1")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") # If compiler is Clang
    if (CMAKE_BUILD_TYPE STREQUAL "Release") # Release build mode
        # Set all this flags explicitly, even if some of them may be covered by -Wall, in order to fine-tune the warning flags we want
        # Note that -Weverything isn't available in some version of Mac and iOS
        # Source: https://github.com/macmade/SeriousCode
        add_compile_options("-Wall" "-Wbad-function-cast" "-Wcast-align" "-Wconversion" "-Wdeclaration-after-statement"
            "-Wdeprecated-implementations" "-Wextra" "-Wfloat-equal" "-Wformat=2" "-Wformat-nonliteral" "-Wfour-char-constants"
            "-Wimplicit-atomic-properties" "-Wmissing-braces" "-Wmissing-declarations" "-Wmissing-field-initializers"
            "-Wmissing-format-attribute" "-Wmissing-noreturn" "-Wmissing-prototypes" "-Wnested-externs" "-Wnewline-eof"
            "-Wold-style-definition" "-Woverlength-strings" "-Wparentheses" "-Wpointer-arith" "-Wredundant-decls"
            "-Wreturn-type" "-Wsequence-point" "-Wshadow" "-Wshorten-64-to-32" "-Wsign-compare" "-Wsign-conversion"
            "-Wstrict-prototypes" "-Wstrict-selector-match" "-Wswitch -Wswitch-default" "-Wswitch-enum" "-Wundeclared-selector"
            "-Wuninitialized" "-Wunknown-pragmas" "-Wunreachable-code" "-Wunused-function" "-Wunused-label" "-Wunused-parameter"
            "-Wunused-value" "-Wunused-variable" "-Wwrite-strings")
    else ()
        # Minimum flags for debug mode
        add_compile_options("-Wall" "-Werror" "-Wextra" "-g")
    endif ()
else ()
    # Unix plateform
    if (UNIX)
        add_compile_options("-std=gnu11" "-pthread" "-ldl")
        if (CMAKE_BUILD_TYPE STREQUAL "Release") # Release build mode
            if (CMAKE_COMPILER_IS_GNUCC) # GNU compiler
                add_compile_options("-O2") # O2 optimization
                set(CMAKE_EXE_LINKER_FLAGS "-s") # Strip binary (only in release mode as it prevent debugging)

                # Hardening options
                ## -fstack-protector-strong
                ### Stack smashing protector
                ## -D_FORTIFY_SOURCE=2
                ### Compile-time protection against static sized buffer overflows.
                ### No known regressions or performance loss
                ## -Wl,-z,now
                ### Disable lazy binding
                ## -Wl,-z,relro
                ### Read-only segments after relocation
                add_compile_options("-D_FORTIFY_SOURCE=2" "-Wl,-z,now" "-Wl,-z,relro")
                if (CMAKE_COMPILER_IS_GNUCC VERSION_GREATER 4.9)
                    add_compile_options("-fstack-protector-strong")
                endif ()
            endif ()
        else () # Debug mode
            add_compile_options("-g") # Add debug symbols
        endif ()
    endif ()
    # It should work on any unix platform ?
    if (CMAKE_BUILD_TYPE STREQUAL "Release") # Release mode
        add_compile_options("$<$<CONFIG:RELEASE>:-O2>" "-fno-builtin")
    endif ()
    ## -Werror=format-security
    ### Reject potentially unsafe format string arguments
    ## -Werror=implicit-function-declaration
    ### Reject missing function prototypes
    #add_compile_options("-Wall" "-Wextra" "-Werror" "-pedantic" "-Werror=format-security" "-Werror=implicit-function-declaration")
    add_compile_options("-Wall" "-Wextra" "-pedantic" "-Werror=format-security" "-Werror=implicit-function-declaration")
endif ()

# CCache

find_program(CCACHE_PROGRAM ccache)
if (CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "${CCACHE_PROGRAM}")
endif()


# --- SEARCH DEPENDENCIES ---

# External dependencies
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/external)

# Location where external projects will be downloaded
set (DOWNLOAD_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/downloads"
    CACHE PATH "Location where external projects will be downloaded.")
mark_as_advanced(DOWNLOAD_LOCATION)

# LibErrorInterceptor
include(libei)

# LibUnknownEchoUtilsModule
include(libueum)

# Search pthread
if (UNIX)
    find_package (Threads)
endif ()

if (NOT LIBEI_SYSTEM)
    list(APPEND LIBUKNOWNECHO_DEPENDENCIES libei)
endif ()

if (NOT LIBUEUM_SYSTEM)
    list(APPEND LIBUKNOWNECHO_DEPENDENCIES libueum)
endif ()

set(LIBSHAREDMEMORYOBJECT_LIBRARIES
    ${LIBERRORINTERCEPTOR_LIBRARIES}
    ${LIBUNKNOWNECHOUTILSMODULE_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)

set(LIBSHAREDMEMORYOBJECT_INCLUDE_DIR
    ${LIBERRORINTERCEPTOR_INCLUDE_DIR}
    ${LIBUNKNOWNECHOUTILSMODULE_INCLUDE_DIR}
)


# --- BUILD LIBSHAREDMEMORYOBJECT ---

set(PROJECT_NAME_STATIC "${PROJECT_NAME}_static")
set(PROJECT_NAME_SHARED "${PROJECT_NAME}_shared")

# Load source files

if (WIN32)
    file(GLOB_RECURSE PROGRAM_TARGET_SRC_FILES "${SRC_PATH}/smo/impl/windows/*.*"
        "${SRC_PATH}/smo/api/*.*" "${SRC_PATH}/smo/utils/*.*")
else ()
    file(GLOB_RECURSE PROGRAM_TARGET_SRC_FILES "${SRC_PATH}/smo/impl/linux/*.*"
        "${SRC_PATH}/smo/api/*.*" "${SRC_PATH}/smo/utils/*.*")
endif ()

# Build shared library
add_library(${PROJECT_NAME_SHARED} SHARED ${PROGRAM_TARGET_SRC_FILES})
target_include_directories(${PROJECT_NAME_SHARED} PUBLIC "${SRC_PATH}")
target_include_directories(${PROJECT_NAME_SHARED} PUBLIC ${LIBSHAREDMEMORYOBJECT_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME_SHARED} ${LIBSHAREDMEMORYOBJECT_LIBRARIES})
set_target_properties(${PROJECT_NAME_SHARED} PROPERTIES OUTPUT_NAME smo)


# Build static library
add_library(${PROJECT_NAME_STATIC} STATIC ${PROGRAM_TARGET_SRC_FILES})
target_include_directories(${PROJECT_NAME_STATIC} PUBLIC "${SRC_PATH}")
set_target_properties(${PROJECT_NAME_STATIC} PROPERTIES OUTPUT_NAME smo_static)
target_include_directories(${PROJECT_NAME_STATIC} PUBLIC ${LIBSHAREDMEMORYOBJECT_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME_STATIC} ${LIBSHAREDMEMORYOBJECT_LIBRARIES})
target_link_libraries(${PROJECT_NAME_STATIC} ${CMAKE_THREAD_LIBS_INIT})


# --- INSTALL ---

# Copy includes
install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include FILES_MATCHING PATTERN "*.h")

# Copy static library
install (
    TARGETS ${PROJECT_NAME_STATIC}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)

if (UNIX)
    install (
        TARGETS ${PROJECT_NAME_SHARED}
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    )
elseif (WIN32)
    install (
        TARGETS ${PROJECT_NAME_SHARED}
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    )
endif ()

# Build hello world example
file(GLOB hello_world_example_path "${EXAMPLES_PATH}/hello_world_example.c")
get_filename_component(hello_world_example ${hello_world_example_path} NAME_WE)
add_executable(${hello_world_example} ${hello_world_example_path})
target_link_libraries(${hello_world_example} ${PROJECT_NAME_STATIC})
target_include_directories(${hello_world_example} PUBLIC ${LIBSHAREDMEMORYOBJECT_INCLUDE_DIR})
target_link_libraries(${hello_world_example} ${LIBSHAREDMEMORYOBJECT_LIBRARIES})
if (UNIX)
    target_link_libraries(${hello_world_example} "-ldl" "-lrt")
endif ()
set_target_properties(${hello_world_example} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${EXAMPLES_OUTPUT_PATH})

# Build hello world library
if (UNIX)
    file(GLOB hello_world_library "${EXAMPLES_PATH}/linux_hello_world_library.c")
elseif (WIN32)
    file(GLOB hello_world_library "${EXAMPLES_PATH}/windows_hello_world_library.c")
endif ()
add_library(hello_world_library SHARED ${hello_world_library})
set_target_properties(hello_world_library PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${EXAMPLES_OUTPUT_PATH})
