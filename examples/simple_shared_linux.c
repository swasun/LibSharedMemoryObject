#include <stdio.h>

void __attribute__ ((constructor)) hello_world(void);

void hello_world(void) {
    fprintf(stdout, "Hello world from shared object !\n");
}
