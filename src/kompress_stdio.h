#ifndef INF3173_KOMPRESS_STDIO_H
#define INF3173_KOMPRESS_STDIO_H

#include "kompress.h"

int kompress_deflate_stdio(struct kompress_app *app);
int kompress_inflate_stdio(struct kompress_app *app);

#endif
