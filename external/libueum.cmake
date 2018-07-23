 ##########################################################################################
 # Copyright (C) 2018 by Charly Lamothe                                                   #
 #                                                                                        #
 # This file is part of LibSharedMemoryObject.                                            #
 #                                                                                        #
 #   LibSharedMemoryObject is free software: you can redistribute it and/or modify        #
 #   it under the terms of the GNU General Public License as published by                 #
 #   the Free Software Foundation, either version 3 of the License, or                    #
 #   (at your option) any later version.                                                  #
 #                                                                                        #
 #   LibSharedMemoryObject is distributed in the hope that it will be useful,             #
 #   but WITHOUT ANY WARRANTY; without even the implied warranty of                       #
 #   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                        #
 #   GNU General Public License for more details.                                         #
 #                                                                                        #
 #   You should have received a copy of the GNU General Public License                    #
 #   along with LibSharedMemoryObject.  If not, see <http://www.gnu.org/licenses/>.       #
 ##########################################################################################

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
