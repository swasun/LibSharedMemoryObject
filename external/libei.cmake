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

 if (LIBEI_SYSTEM)
    if (WIN32)
        set(LIBERRORINTERCEPTOR_INCLUDE_DIR "C:\\LibErrorInterceptor\\$ENV{name}\\include")
        set(LIBERRORINTERCEPTOR_LIBRARIES "C:\\LibErrorInterceptor\\$ENV{name}\\lib\\ei_static.lib")
    elseif (UNIX)
        find_library(LIBERRORINTERCEPTOR_LIBRARIES
            NAMES ei_static libei_static ei libei
            HINTS ${CMAKE_INSTALL_PREFIX}/lib)
        find_path(LIBERRORINTERCEPTOR_INCLUDE_DIR ei)
    endif ()
else (LIBEI_SYSTEM)
    include (ExternalProject)

    set(LIBEI_URL https://github.com/swasun/LibErrorInterceptor.git)
    set(LIBERRORINTERCEPTOR_INCLUDE_DIR ${LIBEI_INSTALL}/external/libei_archive)
    set(LIBEI_BUILD ${ROOT_BUILD_DIR}/libei/src/libei)

    if (WIN32)
        set(LIBERRORINTERCEPTOR_LIBRARIES "${LIBEI_INSTALL}\\lib\\ei_static.lib")
    else()
        set(LIBERRORINTERCEPTOR_LIBRARIES ${LIBEI_INSTALL}/lib/libei_static.a)
    endif()

    ExternalProject_Add(libei
        PREFIX libei
        GIT_REPOSITORY ${LIBEI_URL}
        BUILD_IN_SOURCE 1
        BUILD_BYPRODUCTS ${LIBERRORINTERCEPTOR_LIBRARIES}
        DOWNLOAD_DIR "${DOWNLOAD_LOCATION}"
        CMAKE_CACHE_ARGS
            -DCMAKE_BUILD_TYPE:STRING=Release
            -DCMAKE_INSTALL_PREFIX:STRING=${LIBEI_INSTALL}
            -DCMAKE_C_FLAGS:STRING=-fPIC
    )
endif (LIBEI_SYSTEM)
