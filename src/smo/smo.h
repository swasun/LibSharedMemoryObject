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

#ifndef SHAREDMEMORYOBJECT_MSO_H
#define SHAREDMEMORYOBJECT_MSO_H

#include <smo/api/smo_handle.h>
#include <ueum/ueum.h>

#include <stddef.h>

smo_handle *smo_open(const char *id, unsigned char *data, size_t size);

void *smo_get_function(smo_handle *handle, const char *function_name);

bool smo_close(smo_handle *handle);

#endif
