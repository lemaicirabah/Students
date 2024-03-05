#ifndef INF3173_KOMPRESS_READWRITE_H
#define INF3173_KOMPRESS_READWRITE_H

#include "kompress.h"

int kompress_deflate_readwrite(struct kompress_app *app);
int kompress_inflate_readwrite(struct kompress_app *app);

#endif
