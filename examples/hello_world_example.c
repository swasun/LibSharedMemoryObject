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
#include <smo/utils/alloc.h>
#include <smo/api/smo_handle.h>

#include <ei/ei.h>

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>

#if defined(_MSC_VER)
#include <basetsd.h>
typedef SSIZE_T ssize_t;
#endif

/**
* @brief Get the file size object
*
* @param fd  file descriptor
* @return ssize_t  size of the file
*/
static ssize_t get_file_size(FILE *fd) {
	ssize_t file_size;

	file_size = -1;

	if (!fd) {
		return -1;
	}

	fseek(fd, 0, SEEK_END);
	file_size = ftell(fd);
	fseek(fd, 0, SEEK_SET);

	return file_size;
}

static unsigned char *read_shared_library(const char *path, ssize_t *size) {
	FILE *fd;
	unsigned char *buf;
	ssize_t temp_size;

	fd = NULL;
	buf = NULL;

	/* Open the shared library from the disk */
	if (!(fd = fopen(path, "rb"))) {
		ei_stacktrace_push_errno();
		return NULL;
	}

	/* Get the size of the file */
	if ((temp_size = get_file_size(fd)) == -1) {
		ei_stacktrace_push_errno();
		if (fclose(fd) != 0) {
			ei_logger_warn("Failed to close file descriptor with error message: '%s'", strerror(errno));
		}
		return NULL;
	}

	/* Alloc the correct size for the buffer */
	smo_safe_alloc(buf, unsigned char, temp_size);

	if (!fread(buf, temp_size, 1, fd)) {
		ei_stacktrace_push_errno();
		if (fclose(fd) != 0) {
			ei_logger_warn("Failed to close file descriptor with error message: '%s'", strerror(errno));
		}
		return NULL;
	}

	*size = temp_size;

	if (fclose(fd) != 0) {
		ei_logger_warn("Failed to close file descriptor with error message: '%s'", strerror(errno));
	}

	return buf;
}

int main(int argc, char **argv) {
	unsigned char *buf;
	ssize_t size;
	smo_handle *handle;
	typedef void(*hello_world_func)(void);
	hello_world_func hello_world;

	if (argc != 2) {
		fprintf(stderr, "Usage: ./%s <shared_object_path>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	size = -1;
	buf = NULL;
	handle = NULL;

	/* Initialize LibErrorInterceptor for error handling */
	if (!ei_init()) {
		fprintf(stderr, "[FATAL] Failed to initialize LibErrorInterceptor\n");
		exit(EXIT_FAILURE);
	}

	if (!(buf = read_shared_library(argv[1], &size))) {
		ei_stacktrace_push_msg("Failed to read shared library from disk");
		goto clean_up;
	}

	if (!(handle = smo_open("id", buf, size))) {
		ei_stacktrace_push_msg("Failed to open sharfed memory object from specified buffer");
		goto clean_up;
	}

	if (!(hello_world = smo_get_symbol(handle, "hello_world"))) {
		ei_stacktrace_push_msg("Failed to get symbol of function hello");
		goto clean_up;
	}

	hello_world();

clean_up:
	smo_handle_destroy(handle);
	/* Check if an error was recorded */
	if (ei_stacktrace_is_filled()) {
		ei_logger_stacktrace("An error occurred with the following stacktrace :");
	}
	/* Uninit the error handling library */
	ei_uninit();
	return EXIT_SUCCESS;
}
