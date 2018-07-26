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

if (LIBUEUM_SYSTEM)
    if (WIN32)
        set(LIBUNKNOWNECHOUTILSMODULE_INCLUDE_DIR "C:\\LibUnknownEchoUtilsModule\\$ENV{name}\\include")
        set(LIBUNKNOWNECHOUTILSMODULE_LIBRARIES "C:\\LibUnknownEchoUtilsModule\\$ENV{name}\\lib\\ueum_static.lib")
    elseif (UNIX)
        find_library(LIBUNKNOWNECHOUTILSMODULE_LIBRARIES
            NAMES ueum_static libueum_static ueum libueum
            HINTS ${CMAKE_INSTALL_PREFIX}/lib)
        find_path(LIBUNKNOWNECHOUTILSMODULE_INCLUDE_DIR ueum)
    endif ()
else (LIBUEUM_SYSTEM)
    include (ExternalProject)

    set(LIBUEUM_URL https://github.com/swasun/LibUnknownEchoUtilsModule.git)
    set(LIBUNKNOWNECHOUTILSMODULE_INCLUDE_DIR ${LIBUEUM_INSTALL}/external/libueum_archive)
    set(LIBUEUM_BUILD ${ROOT_BUILD_DIR}/libueum/src/libueum)

    if (WIN32)
        set(LIBUNKNOWNECHOUTILSMODULE_LIBRARIES "${LIBUEUM_INSTALL}\\lib\\ueum_static.lib")
    else()
        set(LIBUNKNOWNECHOUTILSMODULE_LIBRARIES ${LIBUEUM_INSTALL}/lib/libueum_static.a)
    endif()

    ExternalProject_Add(libueum
        PREFIX libueum
        GIT_REPOSITORY ${LIBUEUM_URL}    
        BUILD_IN_SOURCE 1
        BUILD_BYPRODUCTS ${LIBUNKNOWNECHOUTILSMODULE_LIBRARIES}
        DOWNLOAD_DIR "${DOWNLOAD_LOCATION}"
        CMAKE_CACHE_ARGS
            -DCMAKE_BUILD_TYPE:STRING=Release
            -DCMAKE_INSTALL_PREFIX:STRING=${LIBUEUM_INSTALL}
            -DROOT_BUILD_DIR:STRING=${ROOT_BUILD_DIR}
            -DCMAKE_C_FLAGS:STRING=-fPIC
    )
endif (LIBUEUM_SYSTEM)
