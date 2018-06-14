## @brief LibSharedMemoryObject
## @author Charly Lamothe


# --- PROJECT ---

PROJECT(mso C)
cmake_minimum_required(VERSION 3.8)


# --- GLOBAL OPTIONS ---

set(CMAKE_VERBOSE_MAKEFILE on)

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

if (MSVC)
    # Use secure functions by defaualt and suppress warnings about
    #"deprecated" functions
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /D _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /D _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT=1")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /D _CRT_NONSTDC_NO_WARNINGS=1 /D _CRT_SECURE_NO_WARNINGS=1")
endif ()

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")     ## (not debug)
    if(CMAKE_COMPILER_IS_GNUCXX)              ## (GNU COMMAND)
        add_compile_options("-O3")            ### No Optimization
        set(CMAKE_EXE_LINKER_FLAGS "-s")      ### Strip binary (only in release mode)
    endif ()
else ()
    add_compile_options("-g")
endif ()

if (UNIX)
    add_compile_options("-std=gnu99" "-pthread" "-ldl")
elseif (WIN32)
    #add_compile_options("std=c99" "lkernel32" "lntdll")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options("static-libgcc" "ggdb")
    endif ()
else ()
    add_compile_options("-std=c99")
endif ()

if (UNIX)
	add_compile_options("-Wall" "-Werror" "-pedantic" "-fno-builtin")
elseif (WIN32)
	set(CMAKE_C_FLAGS "${CMAKE_ENABLE_C99}")
endif ()

if (WIN32)
    macro(get_WIN32_WINNT version)
    if (WIN32 AND CMAKE_SYSTEM_VERSION)
        set(ver ${CMAKE_SYSTEM_VERSION})
        string(REPLACE "." "" ver ${ver})
        string(REGEX REPLACE "([0-9])" "0\\1" ver ${ver})
        set(${version} "0x${ver}")
    endif()
    endmacro()
    get_WIN32_WINNT(ver)
    add_definitions(-D_WIN32_WINNT=${ver})
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif ()

# CCache

find_program(CCACHE_PROGRAM ccache)
if (CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "${CCACHE_PROGRAM}")
endif()

# --- SEARCH LIB ---

# Search pthread

if (UNIX)
	find_package (Threads)
endif ()

# Search LibErrorInterceptor

if (WIN32)
	set(LIBERRORINTERCEPTOR_INCLUDE "C:\\LibErrorInterceptor\\x86-Release\\include")
endif ()

# --- BUILD LIBSHAREDMEMORYOBJECT ---

# Options
set(PROJECT_NAME_STATIC "${PROJECT_NAME}_static")
set(PROJECT_NAME_SHARED "${PROJECT_NAME}_shared")

# Load source files

if (WIN32)
    file(GLOB_RECURSE PROGRAM_TARGET_SRC_FILES "${SRC_PATH}/smo/windows/*.*"
    smo.h)
else ()
    file(GLOB_RECURSE PROGRAM_TARGET_SRC_FILES "${SRC_PATH}/smo/linux/*.*"
        smo.h)
endif ()

# Build shared library
add_library(${PROJECT_NAME_SHARED} SHARED ${PROGRAM_TARGET_SRC_FILES})
target_include_directories(${PROJECT_NAME_SHARED} PUBLIC "${SRC_PATH}")
set_target_properties(${PROJECT_NAME_SHARED} PROPERTIES OUTPUT_NAME mso)
target_link_libraries(${PROJECT_NAME_SHARED} ${CMAKE_THREAD_LIBS_INIT})
if (UNIX)
	target_link_libraries(${PROJECT_NAME_SHARED} "-lei")
elseif (WIN32)
	target_link_libraries(${PROJECT_NAME_SHARED} ${CMAKE_CURRENT_SOURCE_DIR}/lib/ei.lib)
endif ()


# Build static library
add_library(${PROJECT_NAME_STATIC} STATIC ${PROGRAM_TARGET_SRC_FILES})
target_include_directories(${PROJECT_NAME_STATIC} PUBLIC "${SRC_PATH}")
set_target_properties(${PROJECT_NAME_STATIC} PROPERTIES OUTPUT_NAME mso)
target_link_libraries(${PROJECT_NAME_STATIC} ${CMAKE_THREAD_LIBS_INIT})
if (UNIX)
	target_link_libraries(${PROJECT_NAME_STATIC} "-lei")
elseif (WIN32)
	target_link_libraries(${PROJECT_NAME_STATIC} ${CMAKE_CURRENT_SOURCE_DIR}/lib/ei.lib)
endif ()

# --- INSTALL ---

if (UNIX)
    # Copy includes
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/ DESTINATION /usr/include FILES_MATCHING PATTERN "*.h")

    # Copy static library
    install (TARGETS ${PROJECT_NAME_STATIC} ARCHIVE DESTINATION "/usr/lib")

    # Copy shared library
    install (TARGETS ${PROJECT_NAME_SHARED} LIBRARY DESTINATION "/usr/lib")
elseif (WIN32)
	# Copy includes
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include FILES_MATCHING PATTERN "*.h")

	# Copy static library
	install (
		TARGETS ${PROJECT_NAME_STATIC}
		ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
		LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
	)

	install (
		TARGETS ${PROJECT_NAME_SHARED}
		RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
	)
endif ()

# Build simple example
if (UNIX)
	file(GLOB simple_example_path "${EXAMPLES_PATH}/linux_simple_example.c")
elseif (WIN32)
	file(GLOB simple_example_path "${EXAMPLES_PATH}/linux_simple_example.c")
endif ()
get_filename_component(simple_example ${simple_example_path} NAME_WE)
add_executable(${simple_example} ${simple_example_path})
if (UNIX)
	target_link_libraries(${simple_example} ${PROJECT_NAME_STATIC} "-ldl" "-lrt")
	target_link_libraries(${simple_example} "-lei")
elseif (WIN32)
	target_link_libraries(${simple_example} ${CMAKE_CURRENT_SOURCE_DIR}/lib/ei.lib)
	target_include_directories(${simple_example} PUBLIC "${LIBERRORINTERCEPTOR_INCLUDE}")
endif ()
set_target_properties(${simple_example} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${EXAMPLES_OUTPUT_PATH})

# Build simple library
if (UNIX)
    add_library(simple SHARED "${EXAMPLES_PATH}/linux_shared_example.c")
elseif (WIN32)
    add_library(simple SHARED "${EXAMPLES_PATH}/windows_shared_example.c")
endif ()
set_target_properties(simple PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${EXAMPLES_OUTPUT_PATH})
