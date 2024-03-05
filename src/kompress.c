#include "kompress.h"

#include <string.h>

#include "kompress_mmap.h"
#include "kompress_readwrite.h"
#include "kompress_stdio.h"

struct kompress_lib kompress_libs[] = {
    {
        .name = "stdio",
        .inflate = kompress_inflate_stdio,
        .deflate = kompress_deflate_stdio,
    },
    {
        .name = "readwrite",
        .inflate = kompress_inflate_readwrite,
        .deflate = kompress_deflate_readwrite,
    },
    {
        .name = "mmap",
        .inflate = kompress_inflate_mmap,
        .deflate = kompress_deflate_mmap,
    },
    {
        .name = NULL,
        .inflate = NULL,
        .deflate = NULL,
    },
};

struct kompress_lib* kompress_get_lib(const char* name) {
    for (int i = 0; kompress_libs[i].name != NULL; i++) {
        if (strcmp(kompress_libs[i].name, name) == 0) {
            return &kompress_libs[i];
        }
    }
    return NULL;
}
