## Description

LibSharedMemoryObject provides a simple API to load from memory shared library on both Linux (.so) and Windows (.dll).

## API

Example of use of API without error handling for sake of simplicity.

```c
smo_handle *handle;
typedef void(*hello_world_func)(void);
hello_world_func hello_world;

/* Where buffer and size are the content of your memory library */
handle = smo_open("your_id", buffer, size);

/**
 * hello world is now a ptr of the function hello_world()
 * located in the memory library
 */
hello_world = smo_get_function(handle, "hello_world");

/* Print the hello world message from library */
hello_world();

smo_close(handle);
```

And that's all you need !

## Dependencies

* [LibErrorInterceptor](https://github.com/swasun/LibErrorInterceptor), a lightweight and cross-plateform library to handle stacktrace and logging in C99.
* [LibUnknownEchoUtilsModule](https://github.com/swasun/LibUnknownEchoUtilsModule) Utils module of [LibUnknownEcho](https://github.com/swasun/LibUnknownEcho). Last version.

## Implementation

* The Windows implementation is from [MemoryModule](https://github.com/fancycode/MemoryModule), a library to load DLL from memory.
* The Linux implementation opened a file descriptor in rams with `shm_open()` or the syscall `__NR_memfd_create` based on you Kernel version.

## Cross-plateform

Tested on:
* Windows x86
* Windows 64
* Ubuntu 14.04
* Ubuntu 16.04
