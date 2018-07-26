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

/**
 * @brief 
 * 
 * @file smo_linux.c
 * @author Charly Lamothe
 * @date 2018-06-12
 * @source Inspired from https://x-c3ll.github.io/posts/fileless-memfd_create/
 */

#include <smo/smo.h>

#include <ei/ei.h>

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <dlfcn.h>
#include <fcntl.h>
#include <sys/mman.h> /* for shm_open() */
#include <sys/syscall.h> /* for syscall memfd_create() */
#include <sys/utsname.h> /* to get the kernel version */
#include <dlfcn.h> /* dlsym(), dlclose() */

/* https://code.woboq.org/qt5/include/asm/unistd_64.h.html */
#define __NR_memfd_create 319

static bool check_if_kernel_is_greater_than_3_17() {
    struct utsname buffer;

    errno = 0;
    if (uname(&buffer) != 0) {
        ei_stacktrace_push_errno();
        return false;
    }

    return strcmp(buffer.release, "3.17") >= 0;
}

/* Wrapper to call memfd_create syscall */
static inline int memfd_create(const char *name, unsigned int flags) {
    return syscall(__NR_memfd_create, name, flags);
}

/* Returns a file descriptor where we can write our shared object */
static int open_ramfs(const char *id) {
    int shm_fd;

    /* Using shm_open() if kernel is under version 3.17 */
    if (!check_if_kernel_is_greater_than_3_17()) {
        if ((shm_fd = shm_open(id, O_RDWR | O_CREAT, S_IRWXU)) < 0) {
            ei_stacktrace_push_errno();
            return false;
        }
    }
    else {
        if ((shm_fd = memfd_create(id, 1)) < 0) {
            ei_stacktrace_push_errno();
            return false;
        }
    }

    return shm_fd;
}

static void *load_shared_object(const char *id, int shm_fd) {
    char *path;
    void *shared_object_handle;

    path = NULL;
    shared_object_handle = NULL;

    ueum_safe_alloc(path, char, 1024);

    ei_logger_trace("Trying to load shared object...");

    if (check_if_kernel_is_greater_than_3_17()) {
        snprintf(path, 1024, "/proc/%d/fd/%d", getpid(), shm_fd);
    } else {
        snprintf(path, 1024, "/dev/shm/%s", id);
    }

    if (!(shared_object_handle = dlopen(path, RTLD_LAZY))) {
        ei_stacktrace_push_msg("Failed to load shared object with error message: '%s'", dlerror());
        ueum_safe_free(path);
        return NULL;
    }

    ueum_safe_free(path);

    return shared_object_handle;
}

smo_handle *smo_open(const char *id, unsigned char *data, size_t size) {
    smo_handle *handle;
    void *shared_object_handle;
    int shm_fd;

    ei_check_parameter_or_return(id);
    ei_check_parameter_or_return(data);
    ei_check_parameter_or_return(size > 0);

    shared_object_handle = NULL;
    shm_fd = -1;

    if (!(shm_fd = open_ramfs(id))) {
        ei_stacktrace_push_msg("Failed to open ramfs file descriptor");    
        return NULL;
    }

    if (write(shm_fd, data, size) < 0) {
        ei_stacktrace_push_errno();
        if (close(shm_fd) != 0 && errno != 0) {
            ei_logger_warn("Failed to close shm file descriptor with error message: '%s'", strerror(errno));
        }
        return NULL;
    }

    if (!(shared_object_handle = load_shared_object(id, shm_fd))) {
        ei_stacktrace_push_msg("Failed to load shared object into shm file descriptor");
        if (close(shm_fd) != 0 && errno != 0) {
            ei_logger_warn("Failed to close shm file descriptor with error message: '%s'", strerror(errno));
        }
        return NULL;    
    }

    if (close(shm_fd) != 0 && errno != 0) {
        ei_logger_warn("Failed to close shm file descriptor with error message: '%s'", strerror(errno));
    }

    handle = smo_handle_create(id);
    handle->object = shared_object_handle;

    return handle;
}

void *smo_get_function(smo_handle *handle, const char *function_name) {
    void *symbol;

    ei_check_parameter_or_return(handle);
    ei_check_parameter_or_return(function_name);

    if (!(symbol = dlsym(handle->object, function_name))) {
        ei_stacktrace_push_msg("Failed to get symbol with error message: '%s'", dlerror());
        return NULL;
    }

    return symbol;
}

bool smo_close(smo_handle *handle) {
    if (!handle) {
        ei_logger_warn("smo handle already closed");
        return true;
    }

    if (dlclose(handle->object) != 0) {
        ei_stacktrace_push_msg("Failed to close handle object with error message: '%s'", dlerror());
        smo_handle_destroy(handle);
        return false;
    }

    smo_handle_destroy(handle);

    return true;
}
