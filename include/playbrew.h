#ifndef PLAYBREW_H
#define PLAYBREW_H

#include <stddef.h>

typedef struct {
	void *(*sd_open)(const char *path, int mode);
	void (*sd_close)(void *fh);
	int (*sd_read)(void *fh, void *buf, size_t len);
	void (*printf)(const char *format, ...);
	void (*gfx_clear)(int color);
	void (*gfx_drawLine)(int x1, int x2, int y1, int y2, int width, int color);
	void (*gfx_setPixel)(int x, int y, int color);
} PlayBrewAPI;

#endif
