#ifndef PLAYBREW_H
#define PLAYBREW_H

#include <stddef.h>

typedef struct {
	void *(*fopen)(const char *path, const char *mode);
	int (*fclose)(void *fh);
	size_t (*fread)(void *buf, size_t size, size_t count, void *stream);
	void (*printf)(const char *format, ...);
	void (*gfx_clear)(int color);
	void (*gfx_drawLine)(int x1, int x2, int y1, int y2, int width, int color);
	void (*gfx_setPixel)(int x, int y, int color);
} PlayBrewAPI;

#define ADDR_FOPEN ((void *(*)(const char *, const char *))0x240b9715)
#define ADDR_FCLOSE ((int (*)(void *))0x240b9309)
#define ADDR_FREAD ((size_t (*)(void *, size_t , size_t , void *))0x240b98e9)
#define ADDR_PRINTF ((void (*)(const char *, ...))0x240baf51)
#define ADDR_GFX_CLEAR ((void (*)(int))0x24055d5d)
#define ADDR_GFX_DRAWLINE ((void (*)(int, int, int, int, int, int))0x24055be5)
#define ADDR_GFX_SETPIXEL ((void (*)(int, int, int))0x24055901)

#endif
