#ifndef INF3173_KOMPRESS_MMAP_H
#define INF3173_KOMPRESS_MMAP_H

#include "kompress.h"

int kompress_deflate_mmap(struct kompress_app* app);
int kompress_inflate_mmap(struct kompress_app* app);

#endif
