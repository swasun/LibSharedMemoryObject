#include <smo/smo.h>

#include <ei/ei.h>

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>

#include <dlfcn.h>

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

    fd = NULL;
    buf = NULL;

    /* Open the shared library from the disk */
    if (!(fd = fopen(path, "rb"))) {
        ei_stacktrace_push_errno();
        return NULL;
    }

    /* Get the size of the file */
    if ((*size = get_file_size(fd)) == -1) {
        ei_stacktrace_push_errno();
        if (fclose(fd) != 0) {
            ei_logger_warn("Failed to close file descriptor with error message: '%s'", strerror(errno));
        }
        return NULL;
    }

    /* Alloc the correct size for the buffer */
    smo_safe_alloc(buf, unsigned char, *size);

    if (!fread(buf, *size, 1, fd)) {
        ei_stacktrace_push_errno();
        if (fclose(fd) != 0) {
            ei_logger_warn("Failed to close file descriptor with error message: '%s'", strerror(errno));
        }
        return NULL;
    }

    if (fclose(fd) != 0) {
        ei_logger_warn("Failed to close file descriptor with error message: '%s'", strerror(errno));
    }

    return buf;
}

int main(int argc, char **argv) {
    unsigned char *buf;
    ssize_t size;
    void *shared_object_handle;
    typedef void (*hello_world_func)(void);
    hello_world_func hello_world;

    if (argc != 2) {
        fprintf(stderr, "Usage: ./%s <shared_object_path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    size = -1;
    buf = NULL;
    shared_object_handle = NULL;

    /* Initialize LibErrorInterceptor for error handling */
    if (!ei_init()) {
        fprintf(stderr, "[FATAL] Failed to initialize LibErrorInterceptor\n");
        exit(EXIT_FAILURE);
    }

    if (!(buf = read_shared_library(argv[1], &size))) {
        ei_stacktrace_push_msg("Failed to read shared library from disk");
        goto clean_up;
    }

    if (!(shared_object_handle = smo_open(buf, size))) {
        ei_stacktrace_push_msg("Failed to open sharfed memory object from specified buffer");
        goto clean_up;
    }

    /* Disallow GCC error of Wpedantic flag due to function ptr cast */
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpedantic"
        if (!(hello_world = (hello_world_func)dlsym(shared_object_handle, "hello_world"))) {
            ei_stacktrace_push_msg("Failed to get function ptr with error message: '%s'", dlerror());
            goto clean_up;
        }
        /**
         * At this point the "Hello world" message is already printed,
         * because the function is marked as constructor in the shared library.
         */
    #pragma GCC diagnostic pop
    
    /* Print the message a second time by using the function ptr, to be sure it's worked correctly ! */
	hello_world();

clean_up:
    /* Close the shared object handle */
    if (shared_object_handle) {
        dlclose(shared_object_handle);
    }
    /* Clean-up the buffer of the library */
    smo_safe_free(buf);
    /* Check if an error was recorded */
    if (ei_stacktrace_is_filled()) {
		ei_logger_stacktrace("An error occurred with the following stacktrace :");
	}
    /* Uninit the error handling library */
    ei_uninit();
    return EXIT_SUCCESS;
}
