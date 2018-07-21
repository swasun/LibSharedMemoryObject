/*************************************************************************************
 * Copyright (C) 2018 by Charly Lamothe                                              *
 *                                                                                   *
 * This file is part of LibSharedMemorySlot.                                         *
 *                                                                                   *
 *   LibSharedMemorySlot is free software: you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by            *
 *   the Free Software Foundation, either version 3 of the License, or               *
 *   (at your option) any later version.                                             *
 *                                                                                   *
 *   LibSharedMemorySlot is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of                  *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                   *
 *   GNU General Public License for more details.                                    *
 *                                                                                   *
 *   You should have received a copy of the GNU General Public License               *
 *   along with LibSharedMemorySlot.  If not, see <http://www.gnu.org/licenses/>.    *
 ************************************************************************************/

#ifndef SHAREDMEMORYOBJECT_MSO_H
#define SHAREDMEMORYOBJECT_MSO_H

#include <smo/api/smo_handle.h>
#include <smo/utils/bool.h>

#include <stddef.h>

smo_handle *smo_open(const char *id, unsigned char *data, size_t size);

void *smo_get_function(smo_handle *handle, const char *function_name);

bool smo_close(smo_handle *handle);

#endif
