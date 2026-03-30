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

#define ADDR_SD_OPEN ((void *(*)(const char *, int))0x24067595)
#define ADDR_SD_CLOSE ((void (*)(void *))0x240676fd)
#define ADDR_SD_READ ((int (*)(void *, void *, size_t))0x24067919)
#define ADDR_PRINTF ((void (*)(const char *, ...))0x240baf51)
#define ADDR_GFX_CLEAR ((void (*)(int))0x24055d5d)
#define ADDR_GFX_DRAWLINE ((void (*)(int, int, int, int, int, int))0x24055be5)
#define ADDR_GFX_SETPIXEL ((void (*)(int, int, int))0x24055901)

#endif
