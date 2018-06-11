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
    set(EXAMPLES_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin/release/examples")
elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin/debug")
    set(EXAMPLES_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin/debug/examples")
endif ()


# --- COMPILE OPTIONS ---

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")     ## (not debug)
    if(CMAKE_COMPILER_IS_GNUCXX)              ## (GNU COMMAND)
        add_compile_options("-O3")            ### No Optimization
        set(CMAKE_EXE_LINKER_FLAGS "-s")      ### Strip binary (only in release mode)
    endif ()
else ()
    add_compile_options("-g")
endif ()

if (UNIX)
    add_compile_options("-std=gnu99" "-ldl" "-lrt")
elseif (WIN32)
    add_compile_options("-std=c99" "-lkernel32" "-lntdll")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options("-static-libgcc" "-ggdb")
    endif ()
endif()

add_compile_options("-Wall" "-Werror" "-pedantic")

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
endif ()

# CCache

find_program(CCACHE_PROGRAM ccache)
if (CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "${CCACHE_PROGRAM}")
endif()

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
target_link_libraries(${PROJECT_NAME_SHARED} "-lei")

# Build static library
add_library(${PROJECT_NAME_STATIC} STATIC ${PROGRAM_TARGET_SRC_FILES})
target_include_directories(${PROJECT_NAME_STATIC} PUBLIC "${SRC_PATH}")
set_target_properties(${PROJECT_NAME_STATIC} PROPERTIES OUTPUT_NAME mso)
target_link_libraries(${PROJECT_NAME_STATIC} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(${PROJECT_NAME_STATIC} "-lei")

# --- INSTALL ---

if (UNIX)
    # Copy includes
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/ DESTINATION /usr/include FILES_MATCHING PATTERN "*.h")

    # Copy static library
    install (TARGETS ${PROJECT_NAME_STATIC} ARCHIVE DESTINATION "/usr/lib")

    # Copy shared library
    install (TARGETS ${PROJECT_NAME_SHARED} LIBRARY DESTINATION "/usr/lib")
endif ()

# Build simple example
file(GLOB simple_example_path "${EXAMPLES_PATH}/simple_example.c")
get_filename_component(simple_example_exe ${simple_example_path} NAME_WE)
message(STATUS "Build Program : ${simple_example_exe}")
add_executable(${simple_example_exe} ${simple_example_path})
target_link_libraries(${simple_example_exe} ${PROJECT_NAME_STATIC} "-ldl" "-lrt")
set_target_properties(${simple_example_exe} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${EXAMPLES_OUTPUT_PATH})

# Build simple library
if (UNIX)
    add_library(simple SHARED "${EXAMPLES_PATH}/simple_shared_linux.c")
else ()
    add_library(simple SHARED "${EXAMPLES_PATH}/simple_shared_windows.c")
endif ()
set_target_properties(simple PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${EXAMPLES_OUTPUT_PATH})
