/*******************************************************************************
 * Copyright (C) 2018 Charly Lamothe                                           *
 *                                                                             *
 * This file is part of LibSharedMemoryObject.                                 *
 *                                                                             *
 *   Licensed under the Apache License, Version 2.0 (the "License");           *
 *   you may not use this file except in compliance with the License.          *
 *   You may obtain a copy of the License at                                   *
 *                                                                             *
 *   http://www.apache.org/licenses/LICENSE-2.0                                *
 *                                                                             *
 *   Unless required by applicable law or agreed to in writing, software       *
 *   distributed under the License is distributed on an "AS IS" BASIS,         *
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
 *   See the License for the specific language governing permissions and       *
 *   limitations under the License.                                            *
 *******************************************************************************/

#include <smo/smo.h>
#include <smo/api/smo_handle.h>
#include <smo/impl/windows/MemoryModule.h>

#include <ei/ei.h>

smo_handle *smo_open(const char *id, unsigned char *data, size_t size) {
    smo_handle *handle;
    void *object;
    char *error_buffer;

    ei_check_parameter_or_return(id);
    ei_check_parameter_or_return(data);
    ei_check_parameter_or_return(size > 0);

    if (!(object = MemoryLoadLibrary(data, size))) {
        ei_get_last_werror(error_buffer);
        ei_stacktrace_push_msg("Failed to load library from memory with error message: '%s'", error_buffer);
        return NULL;
    }

    handle = smo_handle_create(id);
    handle->object = object;

    return handle;
}

void *smo_get_function(smo_handle *handle, const char *function_name) {
    void *symbol;
    char *error_buffer;

    ei_check_parameter_or_return(handle);
    ei_check_parameter_or_return(function_name);
    
    if (!(symbol = MemoryGetProcAddress(handle->object, function_name))) {
        ei_get_last_werror(error_buffer);
        ei_stacktrace_push_msg("Failed to get symbol with error message: '%s'", error_buffer);
        return NULL;
    }

    return symbol;
}

bool smo_close(smo_handle *handle) {
    if (!handle) {
        ei_logger_warn("smo handle already closed");
        return true;
    }

    MemoryFreeLibrary(handle->object);

    smo_handle_destroy(handle);

    return true;
}
