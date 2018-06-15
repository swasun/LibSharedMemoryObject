/************************************************************************************
* Copyright (C) 2018 by Charly Lamothe												*
*																					*
* This file is part of LibSharedMemorySlot.                                         *
*																					*
*   LibSharedMemorySlot is free software: you can redistribute it and/or modify     *
*   it under the terms of the GNU General Public License as published by			*
*   the Free Software Foundation, either version 3 of the License, or				*
*   (at your option) any later version.												*
*																					*
*   LibSharedMemorySlot is distributed in the hope that it will be useful,          *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of					*
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the					*
*   GNU General Public License for more details.									*
*																					*
*   You should have received a copy of the GNU General Public License               *
*   along with LibSharedMemorySlot.  If not, see <http://www.gnu.org/licenses/>.    *
************************************************************************************/

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
