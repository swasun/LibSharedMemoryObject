#ifndef SHAREDMEMORYOBJECT_MSO_H
#define SHAREDMEMORYOBJECT_MSO_H

#include <stddef.h>

#include <smo/bool.h>
#include <smo/alloc.h>

void *smo_open(unsigned char *data, size_t size);

#endif
