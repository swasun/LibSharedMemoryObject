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
//#include <sys/stat.h>
#include <sys/syscall.h> /* for syscall memfd_create() */
#include <sys/utsname.h> /* to get the kernel version */

/**/
#define SHM_NAME "IceIceBaby"

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
int open_ramfs() {
	int shm_fd;

	/* Using shm_open() if kernel is under version 3.17 */
	if (!check_if_kernel_is_greater_than_3_17()) {
		if ((shm_fd = shm_open(SHM_NAME, O_RDWR | O_CREAT, S_IRWXU)) < 0) {
			ei_stacktrace_push_errno();
			return false;
		}
	}
	else {
		if ((shm_fd = memfd_create(SHM_NAME, 1)) < 0) {
			ei_stacktrace_push_errno();
			return false;
		}
	}

	return shm_fd;
}

void *load_shared_object(int shm_fd) {
	char *path;
	void *shared_object_handle;

	path = NULL;
	shared_object_handle = NULL;

	smo_safe_alloc(path, char, 1024);

	ei_logger_trace("Trying to load shared object...");

	if (check_if_kernel_is_greater_than_3_17()) {
		snprintf(path, 1024, "/proc/%d/fd/%d", getpid(), shm_fd);
	} else {
		snprintf(path, 1024, "/dev/shm/%s", SHM_NAME);
	}

	if (!(shared_object_handle = dlopen(path, RTLD_LAZY))) {
		ei_stacktrace_push_msg("Failed to load shared object with error message: '%s'", dlerror());
		smo_safe_free(path);
		return NULL;
	}

	smo_safe_free(path);

	return shared_object_handle;
}

void *smo_open(unsigned char *data, size_t size) {
    void *shared_object_handle;
	int shm_fd;

	ei_check_parameter_or_return(data);
	ei_check_parameter_or_return(size > 0);

	shared_object_handle = NULL;
	shm_fd = -1;

	if (!(shm_fd = open_ramfs())) {
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

	if (!(shared_object_handle = load_shared_object(shm_fd))) {
		ei_stacktrace_push_msg("Failed to load shared object into shm file descriptor");
		if (close(shm_fd) != 0 && errno != 0) {
			ei_logger_warn("Failed to close shm file descriptor with error message: '%s'", strerror(errno));
		}
		return NULL;	
	}

	if (close(shm_fd) != 0 && errno != 0) {
		ei_logger_warn("Failed to close shm file descriptor with error message: '%s'", strerror(errno));
	}

	return shared_object_handle;
}
