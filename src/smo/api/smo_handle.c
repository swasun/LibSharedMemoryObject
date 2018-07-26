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

#include <smo/api/smo_handle.h>
#include <ueum/ueum.h>

#include <string.h>

smo_handle *smo_handle_create(const char *id) {
    smo_handle *handle;

    handle = NULL;

    ueum_safe_alloc(handle, smo_handle, 1);
    handle->object = NULL;
    ueum_safe_alloc(handle->id, char, strlen(id) + 1);
    strcpy(handle->id, id);

    return handle;
}

void smo_handle_destroy(smo_handle *handle) {
    if (handle) {
        ueum_safe_free(handle->id);
        ueum_safe_free(handle);
    }
}
